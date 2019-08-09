#pragma once
#include <string>

namespace core
{
namespace platformUtils
{

std::string
getConfigDirectory();

void showWindow(void* handle);
void hideWindow(void* handle);

}
}
