#include "sentinel/Sentinel.h"

#include <iostream>
#include "sentinel/IVendor.h"

namespace sentinel
{

Sentinel::Sentinel(const UpdateFunctor& updateFunctor):
    _updateFunctor(updateFunctor)
{
}

void
Sentinel::addTrackedItem(const TrackItemPtr& item, const IVendorPtr& vendor)
{
    if (!item->isValid()) {
        std::cerr << "Invalid item ignored: " << item->uri() << std::endl;
        return;
    }
    std::scoped_lock<std::shared_mutex> lock(_itemMutex);
    _items.push_back(std::make_pair(item, vendor));
}

void
Sentinel::startTrackingItems(const std::chrono::milliseconds& updateIntervalMs, bool join)
{
    // Kick off master thread which will take care of making sure a pass of all
    // item updates happens every N milliseconds (or as fast as it can).
    _masterThread = std::thread(std::bind(
        &Sentinel::tick,
        this,
        updateIntervalMs));

    if (join) {
        _masterThread.join();
    }
}

void
Sentinel::tick(const std::chrono::milliseconds& updateIntervalMs)
{
    bool isFirst = true;
    while (true && !_items.empty()) {
        const auto start = std::chrono::steady_clock::now();

        {
            std::vector<size_t> indicesToRemove;
            std::scoped_lock<std::shared_mutex> lock(_itemMutex);
            for (size_t i = 0; i < _items.size(); ++i) {
                const auto& itemVendor = _items[i];
                itemVendor.second->updateItem(itemVendor.first, false);
                const bool needsRemove = _updateFunctor(*itemVendor.first, isFirst);
                if (needsRemove) {
                    indicesToRemove.push_back(i);
                }
            }

            for (auto it = indicesToRemove.crbegin(); 
                    it != indicesToRemove.crend(); ++it) {
                _items.erase(_items.begin() + *it);
            }
        }

        const auto end = std::chrono::steady_clock::now();

        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (elapsedMs < updateIntervalMs) {
            std::this_thread::sleep_for(updateIntervalMs - elapsedMs);
        }

        isFirst = false;
    }
}

}
