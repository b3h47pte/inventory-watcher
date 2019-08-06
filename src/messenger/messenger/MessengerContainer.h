#pragma once

#include "messenger/IMessenger.h"
#include "messenger/backend/SMTPClient.h"
#include "messenger/channels/PhoneNumber.h"
#include <vector>

namespace messenger
{

class MessengerContainer: public IMessenger
{
public:
    void notify(const std::string& fullMessage, const std::string& shortMessage) override;

    // Setup backends
    void setupEmailBackendFromConfig();

    // Add messengers
    void addEmailMessenger(const std::string& destination);
    void addSMSMessenger(const PhoneNumber& destination);

private:
    std::vector<IMessengerPtr> _messengers;

    // Backend objects.
    SMTPClientPtr _smtpClient;
};

}
