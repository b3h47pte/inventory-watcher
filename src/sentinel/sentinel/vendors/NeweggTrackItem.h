#include "sentinel/ITrackItem.h"

#include <string>

namespace sentinel
{

class NeweggTrackItem: public ITrackItem
{
public:
    NeweggTrackItem(const std::string& url);

    void staticUpdate() override;
    void update() override;

    bool isValid() const override { return _valid; }
    std::string uri() const override { return _url; }
    std::string name() const override { return _name; }

private:
    bool _valid{false};
    std::string _url;
    std::string _name;

};

}
