#pragma once

#include <memory>
#include "sentinel/IVendorFwd.h"
#include "sentinel/TrackItem.h"
#include <string>

namespace sentinel
{

class IVendor: public std::enable_shared_from_this<IVendor>
{
public:
    virtual ~IVendor() = default;

    virtual std::string name() const = 0;
    virtual VendorSource source() const = 0;

    virtual TrackItemPtr findItemFromName(const std::string& name) const = 0;
    virtual void updateItem(const TrackItemPtr& item, bool staticUpdate) const = 0;
    virtual void checkout(const TrackItemPtr& item) const = 0;
};

}
