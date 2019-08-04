ADD_LIBRARY(sentinel SHARED
    sentinel/Sentinel.cpp
    sentinel/VendorFactory.cpp
    sentinel/backend/HTTPBackend.cpp
    sentinel/backend/HTMLParser.cpp
    sentinel/vendors/NeweggVendor.cpp
    sentinel/vendors/NeweggTrackItem.cpp)

TARGET_INCLUDE_DIRECTORIES(sentinel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR})

TARGET_LINK_LIBRARIES(sentinel ${CURL_LIBRARIES} ${OPENSSL_LIBRARIES} PkgConfig::Gumbo)

INSTALL(TARGETS sentinel
        LIBRARY DESTINATION lib)
