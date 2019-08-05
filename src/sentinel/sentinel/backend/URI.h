#pragma once

#include <string>

namespace sentinel
{

class URI
{
public:

    URI(const std::string& uri);

    const std::string& protocol() const { return _protocol; }
    const std::string& host() const { return _host; }
    const std::string& path() const { return _path; }

    std::string uri() const;

private:
    void parseURI(const std::string& uri);

    std::string _protocol;
    std::string _host;
    std::string _path;
};

}
