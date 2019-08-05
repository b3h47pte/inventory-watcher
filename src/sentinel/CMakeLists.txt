ADD_LIBRARY(sentinel SHARED
    sentinel/Sentinel.cpp
    sentinel/VendorFactory.cpp
    sentinel/backend/Constants.cpp
    sentinel/backend/HTTPBackend.cpp
    sentinel/backend/HTMLParser.cpp
    sentinel/backend/URI.cpp
    sentinel/vendors/NeweggVendor.cpp
    sentinel/vendors/NeweggTrackItem.cpp)

SET_LIBRARY_TARGET_PROPERTIES(sentinel)
ADD_DEPENDENCIES(sentinel libcef_dll_wrapper)
TARGET_INCLUDE_DIRECTORIES(sentinel SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(sentinel 
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    PkgConfig::Gumbo
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS})

INSTALL(TARGETS sentinel
        LIBRARY DESTINATION lib)
