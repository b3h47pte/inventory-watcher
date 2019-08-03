ADD_LIBRARY(messenger SHARED
    messenger/channels/EmailMessenger.cpp)

TARGET_INCLUDE_DIRECTORIES(messenger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
INSTALL(TARGETS messenger
        LIBRARY DESTINATION lib)
