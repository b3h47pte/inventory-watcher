#include "sentinel/TrackItem.h"

namespace sentinel
{

TrackItem::TrackItem(const std::string& url, const VendorSource vendor):
    _vendor(vendor),
    _url(url)
{
    _httpInstance = HTTPBackend::get().createInstanceFromUri(_url);
}

void
TrackItem::update(const TrackItemUpdate& update)
{
    if (update.valid.has_value()) {
        _valid = update.valid.value();
    }

    if (update.changed.has_value()) {
        _changed = update.changed.value();
    }

    if (update.name.has_value()) {
        _name = update.name.value();
    }

    if (update.stock.has_value()) {
        _stock = update.stock.value();
    }
}

}
