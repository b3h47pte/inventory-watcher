# Inventory Watcher

Project to notify you when an item comes back in stock and to proceed to checkout automatically.

## Requirements
- CMake 3
- Compiler with C++17 support
- Mac OSX/Linux

## Dependencies
- Boost
- Curl
- OpenSSL
- Poco
- PkgConfig
- Chromium Embedded Framework
- Gumbo

For the Chromium Embedded Framework, you will need to download the binaries from <http://opensource.spotify.com/cefbuilds/index.html> and put them into the appropriate folder (deps/cef/OS).

## Compilation

    mkdir build && cd build
    cmake ../ -DCMAKE_BUILD_TYPE=BUILD_TYPE_HERE -DCMAKE_INSTALL_PREFIX=INSTALL_PREFIX_HERE -DOPENSSL_ROOT_DIR=PATH_TO_OPENSSL_HERE
    make -jX
    make install

OPENSSL_ROOT_DIR is not required on Linux if it's installed in standard fashion via the distribution's package manager.

## Running

To track an item, use the ``watchercli`` executable. For example:

    cd
    ./watchercli --vendor newegg --item "amd 3900x"

The available options are:
- ``interval``: Number of seconds between stock refresh.
- ``vendor``: The vendor to query the item from. Only ``newegg`` is supported.
- ``item``: The item to find on the vendor website.
- ``email``: An email address to send a notification to if the item comes back in stock. Can be specified multiple times.
- ``sms``: A phone number to send a notification to via SMS if the item comes back in stock. Can be specified multiple times.

