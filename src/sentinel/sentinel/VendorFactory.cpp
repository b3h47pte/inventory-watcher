#include "sentinel/VendorFactory.h"

#include <boost/algorithm/string.hpp>
#include "sentinel/vendors/NeweggVendor.h"

namespace sentinel
{
namespace
{

const std::string kNeweggToken = "newegg";

}

IVendorPtr
VendorFactory::createFromString(const std::string& vendor) const
{
    std::string normalizedString = vendor;
    std::transform(normalizedString.begin(), normalizedString.end(), normalizedString.end(), ::tolower);
    boost::algorithm::trim(normalizedString);

    if (normalizedString == kNeweggToken)
    {
        return std::make_shared<NeweggVendor>(); 
    }

    return nullptr;
}

}
