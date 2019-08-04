#include <boost/program_options.hpp>
#include <iostream>
#include <sentinel/Sentinel.h>
#include <sentinel/VendorFactory.h>
#include <string>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    po::options_description desc("Inventory Watcher CLI");
    desc.add_options()
        ("vendor", po::value<std::string>()->required(), "Which vendor to use to find the item.")
        ("item", po::value<std::string>(), "Which item to find on the vendor.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    sentinel::VendorFactory factory;
    const sentinel::IVendorPtr vendor = factory.createFromString(vm["vendor"].as<std::string>());
    if (!vendor)
    {
        throw std::runtime_error("Failed to find specified vendor.");
    }
    std::cout << "SELECTED VENDOR: " << vendor->name() << std::endl;
    const sentinel::ITrackItemPtr item = vendor->findItemFromName(vm["item"].as<std::string>());
    if (!item || !item->isValid())
    {
        throw std::runtime_error("Failed to find specified item.");
    }
    std::cout << "FOUND ITEM: " << item->uri() << std::endl;

    sentinel::Sentinel sentinelObj([](const sentinel::ITrackItem& item){
    });
    sentinelObj.addTrackedItem(item);

    using namespace std::chrono_literals;
    sentinelObj.startTrackingItems(500ms, true);

    return 0;
}
