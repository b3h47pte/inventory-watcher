ADD_LIBRARY(messenger SHARED
    messenger/MessengerContainer.cpp
    messenger/backend/SMTPClient.cpp
    messenger/channels/EmailMessenger.cpp)

TARGET_INCLUDE_DIRECTORIES(messenger PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
    ${Poco_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(messenger core ${Poco_LIBRARIES} ${OPENSSL_LIBRARIES})
INSTALL(TARGETS messenger
        LIBRARY DESTINATION lib)
