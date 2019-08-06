#pragma once

#include <memory>

namespace sentinel
{

class IVendor;
using IVendorPtr = std::shared_ptr<IVendor>;
using IConstVendorPtr = std::shared_ptr<const IVendor>;

enum class VendorSource
{
    Newegg
};

}
