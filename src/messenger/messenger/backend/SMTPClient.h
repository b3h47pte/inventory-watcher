#pragma once
#include <memory>
#include <Poco/Net/SecureSMTPClientSession.h>
namespace messenger
{

class SMTPClient
{
public:
    SMTPClient(
        const std::string& hostname,
        const uint16_t port,
        const std::string& username,
        const std::string& pw);

    void sendMessageTo(
        const std::string& subject,
        const std::string& message,
        const std::string& destination);

private:
    Poco::Net::SecureSMTPClientSession _smtpSession;
};

using SMTPClientPtr = std::shared_ptr<SMTPClient>;

}
