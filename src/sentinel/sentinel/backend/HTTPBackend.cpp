#include "sentinel/backend/HTTPBackend.h"

namespace sentinel
{

const HTTPBackend&
HTTPBackend::get()
{
    static HTTPBackend backend;
    return backend;
}


std::string
HTTPBackend::performGETRequest(const std::string& url) const
{
    return "";
}

}
