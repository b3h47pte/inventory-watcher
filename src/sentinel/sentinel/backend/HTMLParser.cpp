#include "sentinel/backend/HTMLParser.h"

#include <iostream>

namespace sentinel
{

HTMLParser::HTMLParser(const std::string& html):
    _html(html)
{
    _output = gumbo_parse(_html.c_str());
}

HTMLParser::~HTMLParser()
{
    gumbo_destroy_output(&kGumboDefaultOptions, _output);
}

}

