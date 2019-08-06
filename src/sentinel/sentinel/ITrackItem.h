#pragma once

#include <iostream>
#include <memory>

namespace sentinel
{

enum class InventoryStock
{
    InStock,
    OutStock
};

class ITrackItem
{
public:
    virtual ~ITrackItem() {}

    virtual void staticUpdate() = 0;
    virtual void update() = 0;

    virtual bool isValid() const = 0;
    virtual bool changedSinceLastUpdate() const = 0;
    virtual std::string uri() const = 0;
    virtual std::string name() const = 0;
    virtual InventoryStock stock() const = 0;
    virtual void print(std::ostream& out) const = 0;
};

using ITrackItemPtr = std::shared_ptr<ITrackItem>;

inline
std::ostream&
operator<<(std::ostream& out, const ITrackItem& item)
{
    item.print(out);
    return out;
}

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



}
