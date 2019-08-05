#pragma once
#include <curl/curl.h>
#include <include/wrapper/cef_library_loader.h>
#include "sentinel/backend/URI.h"
#include <string>

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

private:
    CURL* _curl;
};

}
