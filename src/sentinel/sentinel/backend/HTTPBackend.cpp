#include "sentinel/backend/HTTPBackend.h"

#include <include/cef_app.h>
#include <include/cef_client.h>
#include <iostream>
#include <thread>

namespace sentinel
{
namespace
{

// Implement application-level callbacks for the browser process.
class SimpleApp : public CefApp, public CefBrowserProcessHandler {
public:
    SimpleApp() = default;

    // CefApp methods:
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SimpleApp);
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
    return "";
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
    CefScopedLibraryLoader cefLoader;
    if (!cefLoader.LoadInMain()) {
        throw std::runtime_error("Failed to load CEF.");
    }

    // Initialize CEF.
    CefMainArgs mainArgs(argc, argv);
    CefRefPtr<SimpleApp> cefApp(new SimpleApp);

    CefSettings cefSettings;
    cefSettings.command_line_args_disabled = true;
    cefSettings.windowless_rendering_enabled = true;
#ifndef CEF_USE_SANDBOX
    settings.no_sandbox = true;
#endif

    CefInitialize(mainArgs, cefSettings, cefApp.get(), nullptr);
    CefShutdown();
}

}
