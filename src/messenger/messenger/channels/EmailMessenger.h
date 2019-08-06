#include "messenger/IMessenger.h"
#include "messenger/backend/SMTPClient.h"
#include <string>

namespace messenger
{

class EmailMessenger: public IMessenger
{
public:
    EmailMessenger(const std::string& destination, const SMTPClientPtr& smtpClient);
    void notify(const std::string& fullMessage, const std::string& shortMessage) override;

private:
    std::string _destinationEmail;
    SMTPClientPtr _smtpClient;

};

}
