#include "sentinel/vendors/NeweggTrackItem.h"
#include "sentinel/backend/Constants.h"
#include "sentinel/backend/HTMLParser.h"
#include "sentinel/backend/HTTPBackend.h"
#include "sentinel/StringUtility.h"

#include <iostream>

namespace sentinel
{

NeweggTrackItem::NeweggTrackItem(const std::string& url):
    _url(url)
{
}

void
NeweggTrackItem::staticUpdate()
{
    UpdateOptions options;
    options.updateName = true;
    options.updateStock = true;
    internalUpdate(options);
}

void
NeweggTrackItem::update()
{
    UpdateOptions options;
    options.updateName = false;
    options.updateStock = true;
    internalUpdate(options);
}

void
NeweggTrackItem::internalUpdate(const UpdateOptions& options)
{
    _valid = true;
    try {
        const std::string result = HTTPBackend::get().requestHTMLFromUri(_url);
        
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
        parser.dfsSearchWithContext<NeweggParseContext>(ctxt, [this, &options](GumboNode* node, NeweggParseContext& ctxt) -> bool {
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
                    _name = createStringAttr(node->v.text.text);
                    ctxt.foundName = true;
                } else if (ctxt.foundStockContainer && options.updateStock) {
                    const std::string stockText = normalizeString(createStringAttr(node->v.text.text));
                    if (stockText.find("out of stock") != std::string::npos) {
                        _stock =  InventoryStock::OutStock;
                    } else {
                        _stock =  InventoryStock::InStock;
                    }

                    ctxt.foundStock = true;
                }
            }

            return (ctxt.foundName || !options.updateName) && (ctxt.foundStock || !options.updateStock);
        });
    } catch (...) {
        std::cerr << "Failed to update tracked item." << std::endl;
        _valid = false;
    }
}

void
NeweggTrackItem::print(std::ostream& out) const
{
    out << "NEWEGG TRACKED ITEM" << std::endl;
    out << "\tName: " << _name << std::endl;
    out << "\tUrl : " << _url << std::endl;
    out << "\tStock : " << _stock << std::endl;
}

}
