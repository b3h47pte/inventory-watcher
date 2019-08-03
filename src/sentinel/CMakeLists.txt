ADD_LIBRARY(sentinel SHARED
    sentinel/VendorFactory.cpp
    sentinel/vendors/NeweggVendor.cpp
    sentinel/vendors/NeweggTrackItem.cpp)

TARGET_INCLUDE_DIRECTORIES(sentinel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})
INSTALL(TARGETS sentinel
        LIBRARY DESTINATION lib)
