#pragma once

#include <string>

namespace messenger
{

enum class Carrier
{
    ATT, // att
    TMobile, // tmb
    Verizon, // vz
    Sprint, // st
    GoogleFi, // gfi
    Unknown
};

Carrier carrierFromString(const std::string& str);
std::string carrierEmailHost(Carrier carrier);

class PhoneNumber
{
public:
    explicit PhoneNumber(const std::string& fullString);
    PhoneNumber(const std::string& number, Carrier carrier);

    std::string toEmailString() const;

private:
    std::string _number;
    Carrier _carrier;
};

}
