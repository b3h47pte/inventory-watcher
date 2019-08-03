#include <string>

namespace sentinel
{

class HTTPBackend
{

public:
    static const HTTPBackend& get();

    std::string performGETRequest(const std::string& url) const;

};

}
