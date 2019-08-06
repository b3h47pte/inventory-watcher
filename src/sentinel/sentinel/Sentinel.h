#pragma once
#include "sentinel/TrackItem.h"

#include <chrono>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

namespace sentinel
{

class Sentinel
{
public:
    using UpdateFunctor = std::function<void(const TrackItem&, bool)>;

    Sentinel(const UpdateFunctor& updateFunctor);

    void addTrackedItem(const TrackItemPtr& item, const IVendorPtr& vendor);

    void startTrackingItems(const std::chrono::milliseconds& updateIntervalMs, bool join);

private:
    void tick(const std::chrono::milliseconds& updateIntervalMs) const;

    UpdateFunctor _updateFunctor;
    std::vector<std::pair<TrackItemPtr, IVendorPtr>> _items;
    std::thread _masterThread;
    mutable std::shared_mutex _itemMutex;
};

}
