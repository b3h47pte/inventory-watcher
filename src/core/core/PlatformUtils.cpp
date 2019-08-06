#include "core/PlatformUtils.h"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

namespace core
{
namespace platformUtils
{

std::string
getConfigDirectory()
{
#ifdef __APPLE__
    const bfs::path homeDir(std::getenv("HOME"));
    const bfs::path iwConfigDir = homeDir / "Library" / "Application Support" / "InventoryWatcher";
    bfs::create_directory(iwConfigDir);
    return iwConfigDir.native();
#else
    throw std::runtime_error("Unsupported OS.");
    return "";
#endif
}

}
}
