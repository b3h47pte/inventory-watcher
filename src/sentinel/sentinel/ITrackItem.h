#pragma once

#include <memory>

namespace sentinel
{

class ITrackItem
{
public:
    virtual ~ITrackItem() {}

    virtual void staticUpdate() = 0;
    virtual void update() = 0;

    virtual bool isValid() const = 0;
    virtual std::string uri() const = 0;
    virtual std::string name() const = 0;
};

using ITrackItemPtr = std::shared_ptr<ITrackItem>;

}
