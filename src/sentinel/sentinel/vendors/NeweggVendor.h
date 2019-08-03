#pragma once

#include "sentinel/IVendor.h"

namespace sentinel
{

class NeweggVendor: public IVendor
{
public:
    virtual ~NeweggVendor() = default;

    std::string name() const override { return "Newegg"; }

    ITrackItemPtr findItemFromName(const std::string& name) const override;

private:
    std::string findItemUrlFromSearchQuery(const std::string& query) const;
};

}
