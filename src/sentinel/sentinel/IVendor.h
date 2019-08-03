#pragma once

#include <memory>
#include "sentinel/ITrackItem.h"
#include <string>

namespace sentinel
{

class IVendor
{
public:
    virtual ~IVendor() = default;

    virtual std::string name() const = 0;

    virtual ITrackItemPtr findItemFromName(const std::string& name) const = 0;
};

using IVendorPtr = std::shared_ptr<IVendor>;

}
