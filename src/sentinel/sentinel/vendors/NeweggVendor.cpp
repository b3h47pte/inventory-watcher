#include "sentinel/vendors/NeweggVendor.h"

#include <boost/algorithm/string.hpp>
#include "sentinel/backend/HTMLParser.h"
#include "sentinel/backend/HTTPBackend.h"
#include "sentinel/vendors/NeweggTrackItem.h"
#include <sstream>

#include <iostream>

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
    // First, find the div with class "list-wrap" then look for the first
    // div with class "item-container". Then in that div, look for a div
    // with class "item-info". Then grab the URL from that div.
    const HTMLParser parser(result);

    struct NeweggParseContext
    {
        bool foundListWrap{false};
        bool foundItemContainer{false};
        bool foundItem{false};
        std::string itemUrl;
    };

    auto createStringAttr = [](const char* val){
        std::string ret(val);
        boost::algorithm::trim(ret);
        return ret;
    };

    NeweggParseContext ctxt;
    parser.dfsSearchWithContext<NeweggParseContext>(ctxt, [&createStringAttr](GumboNode* node, NeweggParseContext& ctxt) -> bool {
        GumboAttribute* attr;
        if (!ctxt.foundListWrap) {
            // Look for a div with class "list-wrap".
            if (node->v.element.tag == GUMBO_TAG_DIV &&
                    (attr = gumbo_get_attribute(&node->v.element.attributes, "class"))) {
                if (createStringAttr(attr->value) == "list-wrap") {
                    ctxt.foundListWrap = true;
                }
            }
        } else if (!ctxt.foundItemContainer) {
            if (node->v.element.tag == GUMBO_TAG_DIV &&
                    (attr = gumbo_get_attribute(&node->v.element.attributes, "class"))) {
                if (createStringAttr(attr->value) == "item-container") {
                    ctxt.foundItemContainer = true;
                }
            }
        } else if (!ctxt.foundItem) {
            if (node->v.element.tag == GUMBO_TAG_DIV &&
                    (attr = gumbo_get_attribute(&node->v.element.attributes, "class"))) {
                if (createStringAttr(attr->value) == "item-info") {
                    ctxt.foundItem = true;
                }
            }
        } else {
            if (node->v.element.tag == GUMBO_TAG_A &&
                    (attr = gumbo_get_attribute(&node->v.element.attributes, "class"))) {
                if (createStringAttr(attr->value) == "item-title") {
                    attr = gumbo_get_attribute(&node->v.element.attributes, "href");
                    ctxt.itemUrl = attr->value;
                    return true;
                }
            }
        }


        return false;
    });

    // Return the item's URL.
    return ctxt.itemUrl;
}

}
