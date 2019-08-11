#pragma once
#include <condition_variable>
#include <curl/curl.h>
#ifdef __APPLE__
#include <include/wrapper/cef_library_loader.h>
#endif
#include "sentinel/backend/URI.h"
#include "sentinel/backend/HTTPInstance.h"
#include <string>
#include <thread>

namespace sentinel
{

class HTTPBackend
{

public:
    static const HTTPBackend& get();
    static HTTPBackend& getMutable();

    HTTPBackend();
    ~HTTPBackend();

    void initialize(int argc, char** argv);

    std::string escapeString(const std::string& inStr) const;

    std::string requestHTMLFromUri(const URI& uri) const;
    std::string requestHTMLFromInstance(const HTTPInstance& instance) const;
    HTTPInstancePtr createInstanceFromUri(const URI& uri) const;

private:
    CURL* _curl;

#ifdef __APPLE__
    CefScopedLibraryLoader _cefLoader;
#endif

    mutable std::condition_variable _readyCv;
    bool _ready{false};
};

}
