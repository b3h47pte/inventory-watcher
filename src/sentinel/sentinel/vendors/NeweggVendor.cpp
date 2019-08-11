#include "sentinel/vendors/NeweggVendor.h"

#include <boost/algorithm/string.hpp>
#include "core/StringUtility.h"
#include "sentinel/backend/Constants.h"
#include "sentinel/backend/HTMLParser.h"
#include "sentinel/backend/HTTPBackend.h"
#include <sstream>

namespace sentinel
{

TrackItemPtr
NeweggVendor::findItemFromName(const std::string& name) const
{
    const std::string itemUrl = findItemUrlFromSearchQuery(name);
    if (itemUrl.empty()) {
        return nullptr;
    }

    auto item = std::make_shared<TrackItem>(itemUrl, source());
    updateItem(item, true);
    return item;
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

void
NeweggVendor::updateItem(const TrackItemPtr& item, bool staticUpdate) const
{
    if (item->vendor() != source()) {
        std::cerr << "Can not update item " << item->name() << "since it comes from "
                     "a different source" << std::endl;
        return;
    }

    TrackItemUpdate update;
    update.valid  = true;
    update.changed = false;
    
    struct UpdateOptions
    {
        bool updateName{false};
        bool updateStock{false};
    };
    UpdateOptions options;
    options.updateName = staticUpdate;
    options.updateStock = true;

    try {
        item->instance().refresh();
        const std::string result = HTTPBackend::get().requestHTMLFromInstance(item->instance());
        if (result.empty()) {
            throw std::runtime_error("Failed to get HTML for item.");
        }
        
        const HTMLParser parser(result);

        struct NeweggParseContext
        {
            bool foundBox{false};
            bool foundNameContainer{false};
            bool foundStockContainer{false};

            bool foundName{false};
            bool foundStock{false};
        };

        NeweggParseContext ctxt;
        parser.dfsSearchWithContext<NeweggParseContext>(ctxt, [&update, &options](GumboNode* node, NeweggParseContext& ctxt) -> bool {
            GumboAttribute* classAttr = nullptr;
            GumboAttribute* idAttr = nullptr;
            GumboAttribute* itemProp = nullptr; 

            if (node->type == GUMBO_NODE_ELEMENT) {
                classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
                idAttr = gumbo_get_attribute(&node->v.element.attributes, "id");
                itemProp = gumbo_get_attribute(&node->v.element.attributes, "itemprop");

                if (!ctxt.foundBox) {
                    if (classAttr && 
                            node->v.element.tag == GUMBO_TAG_DIV && 
                            createStringAttr(classAttr->value) == "grpArticle") {
                        ctxt.foundBox = true;
                    }
                } else {
                    if (itemProp && createStringAttr(itemProp->value) == "name") {
                        ctxt.foundNameContainer = true;
                    } else if (idAttr && createStringAttr(idAttr->value) == "landingpage-stock") {
                        ctxt.foundStockContainer = true;
                    }
                }
            } else if (node->type == GUMBO_NODE_TEXT) {
                if (ctxt.foundNameContainer && options.updateName) {
                    const std::string tmpName = createStringAttr(node->v.text.text);
                    update.changed = 
                        update.changed.value() ||
                            (update.name.has_value() &&
                                (tmpName != update.name.value()));
                    update.name = tmpName;
                    ctxt.foundName = true;
                } else if (ctxt.foundStockContainer && options.updateStock) {
                    const std::string stockText = core::normalizeString(createStringAttr(node->v.text.text));

                    InventoryStock tmpStock;
                    if (stockText.find("out of stock") != std::string::npos) {
                        tmpStock =  InventoryStock::OutStock;
                    } else {
                        tmpStock =  InventoryStock::InStock;
                    }
                    update.changed = 
                        update.changed.value() ||
                            (update.stock.has_value() &&
                                (tmpStock != update.stock.value()));
                    update.stock = tmpStock;
                    ctxt.foundStock = true;
                }
            }

            return (ctxt.foundName || !options.updateName) && (ctxt.foundStock || !options.updateStock);
        });
    } catch (const std::exception& ex) {
        std::cerr << "Failed to update tracked item." << std::endl;
        std::cerr << "\t" << ex.what() << std::endl;
        update.valid = false;
        update.changed = false;
    }
    item->update(update);
}

void
NeweggVendor::checkout(const TrackItemPtr& item) const
{
    item->freezeForCheckout();
    HTTPInstance& inst = item->instance();
    inst.displayBrowser();

    // Don't need to wait for browser to display to start running Javascript so we can bring
    // the user to the checkout page.
    inst.executeJavascript({
        "var buttonElements = document.getElementsByClassName(\"call-to-action-main-product\")[0];", // Find the element that contains add to cart button
        "buttonElements.children[0].click();",                                                       // Click the button
        "var popupElements = document.getElementsByClassName(\"centerPopup-body\");",                // See if any popup shows up
        "for (let ele of popupElements) { ele.children[0].click();  }",                              // Close popups if any
        "location = \"https://secure.newegg.com/Shopping/ShoppingCart.aspx?Submit=view\";"           // Redirect to shopping cart
    },
    [](){
    }, // On Success
    [](){
    } // On Failure
    );
}

}
