#pragma once
#include "sentinel/ITrackItem.h"

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
    using UpdateFunctor = std::function<void(const ITrackItem& item)>;

    Sentinel(const UpdateFunctor& updateFunctor);

    void addTrackedItem(const ITrackItemPtr& item);

    void startTrackingItems(const std::chrono::milliseconds& updateIntervalMs, bool join);

private:
    void tick(const std::chrono::milliseconds& updateIntervalMs) const;

    UpdateFunctor _updateFunctor;
    std::vector<ITrackItemPtr> _items;
    std::thread _masterThread;
    mutable std::shared_mutex _itemMutex;
};

}
