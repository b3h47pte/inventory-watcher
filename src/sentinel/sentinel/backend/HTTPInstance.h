#pragma once

#include <functional>
#include <memory>
#include "sentinel/backend/URI.h"
#include <vector>

namespace sentinel
{

class HTTPInstanceImpl;
class HTTPInstance
{
public:
    explicit HTTPInstance(const URI& uri);

    void refresh();
    std::string getResult() const;
    
    void displayBrowser();

    using ExeCallback = std::function<void()>;
    void executeJavascript(const std::vector<std::string>& cmds, const ExeCallback& onSuccess, const ExeCallback& onFailure);

private:
    std::shared_ptr<HTTPInstanceImpl> _impl;
};

using HTTPInstancePtr = std::shared_ptr<HTTPInstance>;

}
