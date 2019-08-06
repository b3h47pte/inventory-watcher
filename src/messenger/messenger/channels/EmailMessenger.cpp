#include "messenger/channels/EmailMessenger.h"

#include <iostream>

namespace messenger
{

EmailMessenger::EmailMessenger(const std::string& destination, const SMTPClientPtr& smtpClient):
    _destinationEmail(destination),
    _smtpClient(smtpClient)
{
}

void
EmailMessenger::notify(const std::string& fullMessage, const std::string& shortMessage)
{
    _smtpClient->sendMessageTo(
        shortMessage,
        fullMessage,
        _destinationEmail);
}

}
