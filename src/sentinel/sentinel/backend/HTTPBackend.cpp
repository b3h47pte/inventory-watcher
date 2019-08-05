#include "sentinel/backend/HTTPBackend.h"

#include <include/base/cef_bind.h>
#ifdef __APPLE__
#include <include/cef_application_mac.h>
#endif
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/wrapper/cef_helpers.h>
#include <include/views/cef_browser_view.h>
#include <include/views/cef_window.h>
#include <iostream>

#include <list>

namespace sentinel
{
namespace
{

class InternalBrowserHandler: public CefClient,
                              public CefRenderHandler,
                              public CefLoadHandler
{
public:
    InternalBrowserHandler(std::promise<std::string>& result):
        _result(result)
    {}
        
    CefRefPtr<CefRenderHandler> GetRenderHandler() override
    {
        return this;
    }

    CefRefPtr<CefLoadHandler> GetLoadHandler() override
    {
        return this;
    }

    void OnPaint(
        CefRefPtr<CefBrowser> browser,
        CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects,
        const void* buffer,
        int width,
        int height) override
    {
    }

    void GetViewRect(
        CefRefPtr<CefBrowser> browser,
        CefRect& rect) override
    {
        rect = CefRect(0, 0, 800, 600);
    }

    void OnLoadEnd(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode) override
    {
        if (!frame->IsMain()) {
            return;
        }

        class HTMLVisitor: public CefStringVisitor
        {
        public:
            HTMLVisitor(std::promise<std::string>& result):
                _result(result)
            {}

            void Visit(const CefString& str) override
            {
                _result.set_value(str);
            }

        private:
            IMPLEMENT_REFCOUNTING(HTMLVisitor);
            std::promise<std::string>& _result;
        };

        CefRefPtr<HTMLVisitor> visitor(new HTMLVisitor(_result));
        browser->GetMainFrame()->GetSource(visitor);
    }

private:
    IMPLEMENT_REFCOUNTING(InternalBrowserHandler);

    std::promise<std::string>& _result;
};

class InternalBrowserApp : public CefApp, public CefBrowserProcessHandler {
public:
    InternalBrowserApp() = default;

    // CefApp methods:
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
    {
        return this;
    }

    void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override
    {
        command_line->AppendSwitchWithValue("disable-features", "NetworkService");
        command_line->AppendSwitch("off-screen-rendering-enabled");
    }

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(InternalBrowserApp);
};

}

const HTTPBackend&
HTTPBackend::get()
{
    static HTTPBackend backend;
    return backend;
}

HTTPBackend&
HTTPBackend::getMutable()
{
    return const_cast<HTTPBackend&>(get());
}

std::string
HTTPBackend::escapeString(const std::string& inStr) const
{
    char* curlStr = curl_easy_escape(_curl, inStr.data(), inStr.size());
    const std::string retStr(curlStr);
    curl_free(curlStr);
    return retStr;
}

std::string
HTTPBackend::requestHTMLFromUri(const URI& uri) const
{
    std::promise<std::string> resultPromise;
    std::future<std::string> futureResult = resultPromise.get_future();

    class RequestTask: public CefTask
    {
    public:
        RequestTask(const URI& uri, std::promise<std::string>& result):
            _uri(uri),
            _result(result)
        {}

        void Execute() override
        {
            CEF_REQUIRE_UI_THREAD();

            // SimpleHandler implements browser-level callbacks.
            CefRefPtr<InternalBrowserHandler> handler(new InternalBrowserHandler(_result));

            // Specify CEF browser settings here.
            CefWindowInfo info;
            info.SetAsWindowless(nullptr);

            CefBrowserSettings browserSettings;
            CefBrowserHost::CreateBrowser(
                info,
                handler.get(),
                _uri.uri(),
                browserSettings,
                nullptr,
                nullptr);
        }

    private:
        const URI& _uri;
        std::promise<std::string>& _result;

        IMPLEMENT_REFCOUNTING(RequestTask);
        DISALLOW_COPY_AND_ASSIGN(RequestTask);
    };

    // Need to wait for the CEF message loop to start.
    if (!_ready) {
        std::mutex m;
        std::unique_lock<std::mutex> lock(m);
        _readyCv.wait(lock);
    }

    CefPostTask(TID_UI, new RequestTask(uri, resultPromise));
    futureResult.wait();
    return futureResult.get();
}

HTTPBackend::HTTPBackend()
{
    curl_global_init(CURL_GLOBAL_ALL);
    _curl = curl_easy_init();
}

HTTPBackend::~HTTPBackend()
{
    curl_easy_cleanup(_curl);
    curl_global_cleanup();
}

void
HTTPBackend::initialize(int argc, char** argv)
{
    if (!_cefLoader.LoadInMain()) {
        throw std::runtime_error("Failed to load CEF.");
    }

    // Initialize CEF.
    CefMainArgs mainArgs(argc, argv);
    CefRefPtr<InternalBrowserApp> cefApp(new InternalBrowserApp);

    CefSettings cefSettings;
    cefSettings.command_line_args_disabled = true;
    cefSettings.windowless_rendering_enabled = true;
#ifndef CEF_USE_SANDBOX
    cefSettings.no_sandbox = true;
#endif
    if (CefExecuteProcess(mainArgs, cefApp.get(), nullptr) >= 0) {
        return;
    }

    CefInitialize(mainArgs, cefSettings, cefApp.get(), nullptr);
    _ready = true;
    _readyCv.notify_all();
    CefRunMessageLoop(); 
    CefShutdown();
}

}
