ADD_EXECUTABLE(watchercli main.cpp)

TARGET_LINK_LIBRARIES(watchercli messenger sentinel Boost::program_options)
TARGET_INCLUDE_DIRECTORIES(watchercli PRIVATE $<TARGET_PROPERTY:sentinel,INTERFACE_INCLUDE_DIRECTORIES>)
INSTALL(TARGETS watchercli
        RUNTIME DESTINATION bin)
