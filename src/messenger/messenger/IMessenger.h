#pragma once
#include <memory>
namespace messenger
{

class IMessenger
{
public:
    virtual ~IMessenger() {}
    virtual void notify(const std::string& fullMessage, const std::string& shortMessage) = 0;
};

using IMessengerPtr = std::shared_ptr<IMessenger>;

}
