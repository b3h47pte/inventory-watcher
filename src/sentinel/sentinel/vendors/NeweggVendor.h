#pragma once

#include "sentinel/IVendor.h"

namespace sentinel
{

class NeweggVendor: public IVendor
{
public:
    virtual ~NeweggVendor() = default;

    std::string name() const override { return "Newegg"; }
    VendorSource source() const override { return VendorSource::Newegg; }
    void updateItem(const TrackItemPtr& item, bool staticUpdate) const override;
    void checkout(const TrackItemPtr& item) const override;

    TrackItemPtr findItemFromName(const std::string& name) const override;

private:
    std::string findItemUrlFromSearchQuery(const std::string& query) const;
};

}
