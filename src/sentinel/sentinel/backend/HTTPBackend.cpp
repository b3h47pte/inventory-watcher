#include "sentinel/backend/HTTPBackend.h"

#include <include/base/cef_bind.h>
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/views/cef_browser_view.h>
#include <include/views/cef_window.h>
#include <iostream>
#include <list>
#ifdef __APPLE__
#include "sentinel/backend/MacHTTPBackend.h"
#endif

namespace sentinel
{
namespace
{

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
    const auto instance = createInstanceFromUri(uri);
    return requestHTMLFromInstance(*instance);
}

std::string
HTTPBackend::requestHTMLFromInstance(const HTTPInstance& instance) const
{
    return instance.getResult();
}

HTTPInstancePtr
HTTPBackend::createInstanceFromUri(const URI& uri) const
{
    if (!_ready) {
        std::mutex m;
        std::unique_lock<std::mutex> lock(m);
        _readyCv.wait(lock);
    }
    return std::make_shared<HTTPInstance>(uri);
}

HTTPBackend::HTTPBackend()
{
    curl_global_init(CURL_GLOBAL_ALL);
    _curl = curl_easy_init();

#ifdef __APPLE__
    initializeMacApp();
#endif
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
#ifndef CEF_USE_SANDBOX
    cefSettings.no_sandbox = true;
#endif
#ifndef NDEBUG
    cefSettings.remote_debugging_port = 8888;
#endif

    cefSettings.log_severity = LOGSEVERITY_ERROR;
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
