#include "messenger/channels/SMSMessenger.h"

namespace messenger
{

SMSMessenger::SMSMessenger(const PhoneNumber& destination, const SMTPClientPtr& smtpClient):
    _destination(destination),
    _emailMessenger(_destination.toEmailString(), smtpClient)
{
}

void
SMSMessenger::notify(const std::string& fullMessage, const std::string& shortMessage)
{
    _emailMessenger.notify(shortMessage, "");
}

}
