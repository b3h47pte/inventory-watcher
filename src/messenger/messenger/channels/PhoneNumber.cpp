#include "messenger/channels/PhoneNumber.h"

#include <boost/algorithm/string.hpp>
#include <core/StringUtility.h>
#include <sstream>
#include <vector>

namespace messenger
{

Carrier
carrierFromString(const std::string& str)
{
    const std::string cmpStr = core::normalizeString(str);
    if (cmpStr == "att") {
        return Carrier::ATT;
    } else if (cmpStr == "tmb") {
        return Carrier::TMobile;
    } else if (cmpStr == "vz") {
        return Carrier::Verizon;
    } else if (cmpStr == "st") {
        return Carrier::Sprint;
    } else if (cmpStr == "gfi") {
        return Carrier::GoogleFi;
    } 

    throw std::runtime_error("Unknown Carrier: " + cmpStr);
    return Carrier::Unknown;
}

std::string
carrierEmailHost(Carrier carrier)
{
    switch (carrier) {
        case Carrier::ATT:
            return "txt.att.net";
        case Carrier::TMobile:
            return "tmomail.net";
        case Carrier::Verizon:
            return "vtext.com";
        case Carrier::Sprint:
            return "messaging.sprintpcs.com";
        case Carrier::GoogleFi:
            return "msg.fi.google.com";
        default:
            break;
    }

    throw std::runtime_error("Unknown Carrier");
    return "";
}

PhoneNumber::PhoneNumber(const std::string& fullString)
{
    std::vector<std::string> tokens;
    boost::algorithm::split(tokens, fullString, boost::is_any_of(":"), boost::token_compress_on);

    _carrier = carrierFromString(tokens[0]);
    _number = tokens[1];
}

PhoneNumber::PhoneNumber(const std::string& number, Carrier carrier):
    _number(number),
    _carrier(carrier)
{
}

std::string
PhoneNumber::toEmailString() const
{
    std::ostringstream str;
    str << _number << "@" << carrierEmailHost(_carrier);
    return str.str();
}

}
