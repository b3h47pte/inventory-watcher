#include <string>
#include <curl/curl.h>

namespace sentinel
{

class HTTPBackend
{

public:
    static const HTTPBackend& get();

    HTTPBackend();
    ~HTTPBackend();

    std::string escapeString(const std::string& inStr) const;

    std::string performGETRequest(const std::string& host, const std::string& target) const;
    std::string performGETRequestSSL(const std::string& host, const std::string& target) const;

private:
    CURL* _curl;

};

}
