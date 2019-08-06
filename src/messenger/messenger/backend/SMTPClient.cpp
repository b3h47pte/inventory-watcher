#include "messenger/backend/SMTPClient.h"
#include <iostream>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/NetException.h>

namespace messenger
{

SMTPClient::SMTPClient(
    const std::string& hostname,
    const uint16_t port,
    const std::string& username,
    const std::string& pw):
    _smtpSession(hostname, port)
{
    std::cout << "Connecting SMTP Client to " << hostname << ":" << port <<  " as " << username << std::endl;
    try {
        _smtpSession.login();
        _smtpSession.startTLS();
        _smtpSession.login(
            Poco::Net::SMTPClientSession::AUTH_LOGIN,
            username,
            pw);
    } catch (const Poco::Net::NetException& ex) {
        std::cout << ex.displayText() << std::endl;
        throw ex;
    }
}

void
SMTPClient::sendMessageTo(
    const std::string& subject,
    const std::string& message,
    const std::string& destination)
{
    Poco::Net::MailMessage msg;
    msg.setSubject(subject);
    msg.setContent(message);
    msg.setSender(destination);

    _smtpSession.sendMessage(
        msg,
        {destination});
}

}
