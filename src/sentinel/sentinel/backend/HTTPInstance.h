#pragma once

#include <memory>
#include "sentinel/backend/URI.h"

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

private:
    std::shared_ptr<HTTPInstanceImpl> _impl;
};

using HTTPInstancePtr = std::shared_ptr<HTTPInstance>;

}
