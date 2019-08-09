#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include "sentinel/backend/HTTPBackend.h"
#include "sentinel/IVendorFwd.h"

namespace sentinel
{

enum class InventoryStock
{
    InStock,
    OutStock
};

struct TrackItemUpdate
{
    std::optional<bool> valid;
    std::optional<bool> changed;
    std::optional<std::string> name;
    std::optional<InventoryStock> stock;
};

class TrackItem
{
public:
    TrackItem(const std::string& url, const VendorSource vendor);

    bool isValid() const { return _valid; }
    bool changedSinceLastUpdate() const { return _changed; }
    std::string uri() const { return _url; }
    std::string name() const { return _name; }
    InventoryStock stock() const { return _stock; }
    VendorSource vendor() const { return _vendor; }

    HTTPInstance& instance() const { return *_httpInstance; }
    void update(const TrackItemUpdate& update);

    bool checkedOut() const { return _checkedOut; }
    void freezeForCheckout() { _checkedOut = true; }

private:
    const VendorSource _vendor;
    bool _valid{false};
    bool _changed{false};
    bool _checkedOut{false};
    const std::string _url;
    std::string _name;
    InventoryStock _stock;
    HTTPInstancePtr _httpInstance;
};

using TrackItemPtr = std::shared_ptr<TrackItem>;

inline
std::ostream&
operator<<(std::ostream& out, InventoryStock stock)
{
    switch (stock)
    {
    case InventoryStock::InStock:
        out << "In Stock";
        break;
    case InventoryStock::OutStock:
        out << "Out of Stock";
        break;
    }
    return out;
}

inline
std::ostream&
operator<<(std::ostream& out, const TrackItem& item)
{
    out << "TRACKED ITEM" << std::endl;
    out << "\tName: " << item.name() << std::endl;
    out << "\tUrl : " << item.uri() << std::endl;
    out << "\tStock : " << item.stock() << std::endl;
    return out;
}

}
