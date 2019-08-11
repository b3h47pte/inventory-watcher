SET(RESOURCES "")
SET(RESOURCE_PREFIX "")
IF(APPLE)
    SET(RESOURCES
        mac/Info.plist
        mac/watchercli.icns
        mac/English.lproj/InfoPlist.strings
        mac/English.lproj/MainMenu.xib)
    SET(RESOURCE_PREFIX "mac/")
    APPEND_PLATFORM_SOURCES(RESOURCES)

    ADD_EXECUTABLE(watchercli MACOSX_BUNDLE main.cpp ${RESOURCES})
    SET_TARGET_PROPERTIES(watchercli PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist
    )
    CEF_CREATE_HELPERS(watchercli cefHelper.cpp ${CMAKE_CURRENT_SOURCE_DIR}/mac/helper-Info.plist)
ELSE()
    ADD_EXECUTABLE(watchercli main.cpp)
ENDIF()
SET_EXECUTABLE_TARGET_PROPERTIES(watchercli)
ADD_DEPENDENCIES(watchercli libcef_dll_wrapper)

TARGET_LINK_LIBRARIES(watchercli
    messenger
    sentinel
    ${Boost_LIBRARIES}
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS})

TARGET_INCLUDE_DIRECTORIES(watchercli PRIVATE 
    $<TARGET_PROPERTY:sentinel,INTERFACE_INCLUDE_DIRECTORIES>
    ${Boost_INCLUDE_DIRS})
INSTALL_TARGET_APP(watchercli "${RESOURCES}" "${RESOURCE_PREFIX}")
