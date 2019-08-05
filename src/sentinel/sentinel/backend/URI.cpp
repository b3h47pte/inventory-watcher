#include "sentinel/backend/URI.h"

#include <sstream>

namespace sentinel
{

URI::URI(const std::string& uri)
{
    parseURI(uri);
}

void
URI::parseURI(const std::string& uri)
{
    // Find Until first colon.
    const auto colonPos = uri.find("://");
    if (colonPos != std::string::npos) {
        _protocol = uri.substr(0, colonPos);
    }
    else {
        _protocol = "http";
    }

    // Find until the first slash
    const auto hostStart =  (colonPos != std::string::npos) ? colonPos + 3 : 0;
    const auto hostEnd = uri.find("/", hostStart);
    _host = uri.substr(hostStart, 
        (hostEnd != std::string::npos) ? hostEnd - hostStart : std::string::npos);

    // Go until the very end
    const auto pathStart = (hostEnd != std::string::npos) ? hostEnd : std::string::npos;
    _path = (pathStart != std::string::npos) ?
        uri.substr(pathStart) : "/";
}

std::string
URI::uri() const
{
    std::ostringstream st;
    st << _protocol << "://" << _host << _path;
    return st.str();
}

}
