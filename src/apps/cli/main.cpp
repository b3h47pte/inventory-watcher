#include <boost/program_options.hpp>
#include <iostream>
#include <messenger/backend/SMTPClient.h>
#include <messenger/channels/EmailMessenger.h>
#include <messenger/MessengerContainer.h>
#include <sentinel/backend/HTTPBackend.h>
#include <sentinel/Sentinel.h>
#include <sentinel/VendorFactory.h>
#include <sstream>
#include <string>
#include <vector>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    // Separate thread that deals with work as we'll block on CEF
    // when the HTTPBackend becomes initialized.
    std::thread workThread([argc, argv](){
        po::options_description desc("Inventory Watcher CLI");
        desc.add_options()
            ("interval", po::value<size_t>()->default_value(15), "Refresh interval in seconds.")
            ("vendor", po::value<std::vector<std::string>>()->required(), "Which vendor(s) to use to find the item.")
            ("item", po::value<std::string>(), "Which item to find on the vendor(s).")
            ("email", po::value<std::vector<std::string>>(), "Email addresses to notify.");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        messenger::MessengerContainer messengers;
        if (vm.count("email") > 0) {
            messengers.setupEmailBackendFromConfig();
            for (const auto& em : vm["email"].as<std::vector<std::string>>()) {
                messengers.addEmailMessenger(em);
            }
        }

        sentinel::Sentinel sentinelObj([&messengers](const sentinel::ITrackItem& item){
            std::ostringstream msg;
            msg << item;

            std::ostringstream shortMsg;
            shortMsg << item.name() << "(" << item.stock() << ")";
            messengers.notify(msg.str(), shortMsg.str());
            std::cout << item << std::endl;
        });

        for (const auto& vendorName : vm["vendor"].as<std::vector<std::string>>()) {
            sentinel::VendorFactory factory;
            const sentinel::IVendorPtr vendor = factory.createFromString(vendorName);
            if (!vendor) {
                throw std::runtime_error("Failed to find specified vendor.");
            }
            std::cout << "SELECTED VENDOR: " << vendor->name() << std::endl;
            const sentinel::ITrackItemPtr item = vendor->findItemFromName(vm["item"].as<std::string>());
            std::cout << "FOUND ITEM: " << item->uri() << std::endl;
            sentinelObj.addTrackedItem(item);
            std::cout << *item << std::endl;
        }

        using namespace std::chrono_literals;
        sentinelObj.startTrackingItems(std::chrono::seconds(vm["interval"].as<size_t>()), true);
    });

    sentinel::HTTPBackend::getMutable().initialize(argc, argv);
    workThread.join();
    return 0;
}
