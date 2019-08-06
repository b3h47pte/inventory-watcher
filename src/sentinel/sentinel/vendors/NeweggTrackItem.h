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
    bool changedSinceLastUpdate() const override { return _changed; }
    std::string uri() const override { return _url; }
    std::string name() const override { return _name; }
    InventoryStock stock() const override { return _stock; }
    void print(std::ostream& out) const override;

private:

    struct UpdateOptions
    {
        bool updateName{false};
        bool updateStock{false};
    };

    void internalUpdate(const UpdateOptions& options);

    bool _valid{false};
    bool _changed{false};
    std::string _url;
    std::string _name;
    InventoryStock _stock;
};

}
