#pragma once
#include "messenger/IMessenger.h"
#include "messenger/channels/EmailMessenger.h"
#include "messenger/channels/PhoneNumber.h"
#include <string>

namespace messenger
{

class SMSMessenger: public IMessenger
{
public:
    SMSMessenger(const PhoneNumber& destination, const SMTPClientPtr& smtpClient);
    void notify(const std::string& fullMessage, const std::string& shortMessage) override;

private:
    PhoneNumber _destination;
    EmailMessenger _emailMessenger;
};

using SMSMessengerPtr = std::shared_ptr<SMSMessenger>;

}
