#include "core/Config.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "core/PlatformUtils.h"
#include <sstream>

namespace bfs = boost::filesystem;
namespace bpt = boost::property_tree;

namespace core
{

const Config&
Config::get()
{
    static Config config = [](){
        const bfs::path pth = bfs::path(platformUtils::getConfigDirectory()) / ".iwconfig";

        if (!bfs::exists(pth)) {
            std::ostringstream err;
            err << "Failed to load config: " << pth.native();
            throw std::runtime_error(err.str());
        }

        Config cfg(pth.native());
        return cfg;
    }();
    return config;
}

Config::Config(const std::string& fname)
{
    bpt::read_json(fname, _data);
}

}
