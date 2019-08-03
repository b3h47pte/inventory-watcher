#include "sentinel/vendors/NeweggVendor.h"

#include "sentinel/backend/HTTPBackend.h"
#include "sentinel/vendors/NeweggTrackItem.h"
#include <sstream>

namespace sentinel
{
namespace
{
const std::string kNeweggURI = "www.newegg.com";
}

ITrackItemPtr
NeweggVendor::findItemFromName(const std::string& name) const
{
    const std::string itemUrl = findItemUrlFromSearchQuery(name);
    return std::make_shared<NeweggTrackItem>(itemUrl);
}

std::string
NeweggVendor::findItemUrlFromSearchQuery(const std::string& query) const
{
    // Do a GET request to perform a search for the given query.
    std::ostringstream target;
    target << "/p/pl?d=" << HTTPBackend::get().escapeString(query);
    const std::string result = HTTPBackend::get().performGETRequestSSL(kNeweggURI, target.str());
    
    // Find the first item returned by parsing the returned HTML.

    // Return the item's URL.
    return "";
}

}
