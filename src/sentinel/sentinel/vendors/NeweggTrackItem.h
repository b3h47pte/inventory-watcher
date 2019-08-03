#include "sentinel/ITrackItem.h"

namespace sentinel
{

class NeweggTrackItem: public ITrackItem
{
public:
    NeweggTrackItem(const std::string& url);

    bool isValid() const override { return false; }

};

}
