#pragma once
#include <functional>
#include <string>
#include <gumbo.h>

namespace sentinel
{

class HTMLParser
{
public:
    HTMLParser(const std::string& html);
    ~HTMLParser();

    template<typename TContext>
    void dfsSearchWithContext(
        TContext& context,
        const std::function<bool(GumboNode* node, TContext& context)>& functor) const;

private:
    template<typename TContext>
    bool dfsSearchWithContextHelper(
        GumboNode* node,
        TContext& context,
        const std::function<bool(GumboNode* node, TContext& context)>& functor) const;

    std::string _html;
    GumboOutput* _output;
};

template<typename TContext>
void
HTMLParser::dfsSearchWithContext(
    TContext& context,
    const std::function<bool(GumboNode* node, TContext& context)>& functor) const
{
    dfsSearchWithContextHelper(
        _output->root,
        context,
        functor);
}

template<typename TContext>
bool
HTMLParser::dfsSearchWithContextHelper(
    GumboNode* node,
    TContext& context,
    const std::function<bool(GumboNode* node, TContext& context)>& functor) const
{
    if (node->type != GUMBO_NODE_ELEMENT) {
        return false;
    }

    const bool done = functor(node, context);
    if (done) {
        return true;
    }

    GumboVector* children = &node->v.element.children;
    for (auto i = 0; i < children->length; ++i) {
        // Need to make a copy so if we find a node in one subtree, we don't
        // believe that another subtree found that node.
        TContext childContext = context;
        if (dfsSearchWithContextHelper(
                static_cast<GumboNode*>(children->data[i]),
                childContext,
                functor)) {

            context = childContext;
            return true;
        }
    }

    return false;
}

}
