#pragma once

#include <boost/algorithm/string.hpp>
#include <string>

namespace core
{

inline
std::string
normalizeString(const std::string& input)
{
    std::string ret = input;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
    boost::algorithm::trim(ret);
    return ret;
}

}
