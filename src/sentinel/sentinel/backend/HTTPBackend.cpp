#include "sentinel/backend/HTTPBackend.h"

#include <include/base/cef_bind.h>
#ifdef __APPLE__
#include <include/cef_application_mac.h>
#endif
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_closure_task.h>
#include <iostream>

namespace sentinel
{
namespace
{

// Implement application-level callbacks for the browser process.
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
    }

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(InternalBrowserApp);
};

void
killCef()
{
    CefQuitMessageLoop();
}

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
    return "";
}

HTTPBackend::HTTPBackend()
{
    curl_global_init(CURL_GLOBAL_ALL);
    _curl = curl_easy_init();
}

HTTPBackend::~HTTPBackend() noexcept(false)
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

    _cefThread = std::thread([argc, argv](){
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
        CefRunMessageLoop(); 
        CefShutdown();
    });
}

void
HTTPBackend::cleanup()
{
    // Shutdown CEF.
    CefPostTask(TID_UI, base::Bind(&killCef));
    _cefThread.join();
}

}
