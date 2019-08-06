#pragma once
#include <condition_variable>
#include <curl/curl.h>
#include <include/wrapper/cef_library_loader.h>
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
    CefScopedLibraryLoader _cefLoader;

    mutable std::condition_variable _readyCv;
    bool _ready{false};
};

}
