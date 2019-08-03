#pragma once

#include <memory>

namespace sentinel
{

class ITrackItem
{
public:
    virtual ~ITrackItem() {}

    virtual bool isValid() const = 0;
};

using ITrackItemPtr = std::shared_ptr<ITrackItem>;

}
