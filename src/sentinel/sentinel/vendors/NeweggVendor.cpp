#include "sentinel/vendors/NeweggVendor.h"

#include <boost/algorithm/string.hpp>
#include "sentinel/backend/Constants.h"
#include "sentinel/backend/HTMLParser.h"
#include "sentinel/backend/HTTPBackend.h"
#include "sentinel/vendors/NeweggTrackItem.h"
#include <sstream>

#include <iostream>

namespace sentinel
{

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
    target << "https://" << kNeweggURI << "/p/pl?d=" << HTTPBackend::get().escapeString(query);
    const std::string result = HTTPBackend::get().requestHTMLFromUri(target.str());
    
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

    NeweggParseContext ctxt;
    parser.dfsSearchWithContext<NeweggParseContext>(ctxt, [](GumboNode* node, NeweggParseContext& ctxt) -> bool {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return false;
        }

        GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "class");
        if (!attr) {
            return false;
        }

        if (!ctxt.foundListWrap) {
            // Look for a div with class "list-wrap".
            if (node->v.element.tag == GUMBO_TAG_DIV) {
                if (createStringAttr(attr->value) == "list-wrap") {
                    ctxt.foundListWrap = true;
                }
            }
        } else if (!ctxt.foundItemContainer) {
            if (node->v.element.tag == GUMBO_TAG_DIV) {
                if (createStringAttr(attr->value) == "item-container") {
                    ctxt.foundItemContainer = true;
                }
            }
        } else if (!ctxt.foundItem) {
            if (node->v.element.tag == GUMBO_TAG_DIV) {
                if (createStringAttr(attr->value) == "item-info") {
                    ctxt.foundItem = true;
                }
            }
        } else {
            if (node->v.element.tag == GUMBO_TAG_A) {
                if (createStringAttr(attr->value) == "item-title") {
                    attr = gumbo_get_attribute(&node->v.element.attributes, "href");
                    ctxt.itemUrl = createStringAttr(attr->value);
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
