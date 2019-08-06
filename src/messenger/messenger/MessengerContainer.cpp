#include "messenger/MessengerContainer.h"

#include "core/Config.h"
#include "messenger/channels/EmailMessenger.h"

namespace messenger
{

void
MessengerContainer::notify(const std::string& fullMessage, const std::string& shortMessage)
{
    for (const auto& msg : _messengers) {
        msg->notify(fullMessage, shortMessage);
    }
}

void
MessengerContainer::setupEmailBackendFromConfig()
{
    const core::Config& config = core::Config::get();
    _smtpClient = std::make_shared<SMTPClient>( 
        config.getVal<std::string>("smtp.hostname"),
        config.getVal<uint16_t>("smtp.port"),
        config.getVal<std::string>("smtp.username"),
        config.getVal<std::string>("smtp.password"));
}

void
MessengerContainer::addEmailMessenger(const std::string& destination)
{
    _messengers.push_back(std::make_shared<EmailMessenger>(destination, _smtpClient));
}

}
