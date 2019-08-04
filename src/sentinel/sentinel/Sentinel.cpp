#include "sentinel/Sentinel.h"

namespace sentinel
{

Sentinel::Sentinel(const UpdateFunctor& updateFunctor):
    _updateFunctor(updateFunctor)
{
}

void
Sentinel::addTrackedItem(const ITrackItemPtr& item)
{
    item->staticUpdate();
    std::scoped_lock<std::shared_mutex> lock(_itemMutex);
    _items.push_back(item);
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
Sentinel::tick(const std::chrono::milliseconds& updateIntervalMs) const
{
    while (true) {
        const auto start = std::chrono::steady_clock::now();

        for (const auto& item : _items)
        {
            item->update();
            _updateFunctor(*item);
        }

        const auto end = std::chrono::steady_clock::now();

        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (elapsedMs < updateIntervalMs) {
            std::this_thread::sleep_for(updateIntervalMs - elapsedMs);
        }
    }
}

}
