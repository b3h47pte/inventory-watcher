#pragma once
#include "sentinel/IVendor.h"
#include <string>

namespace sentinel
{

class VendorFactory
{
public:
    virtual ~VendorFactory() {}
    virtual IVendorPtr createFromString(const std::string& vendor) const;
};

}
