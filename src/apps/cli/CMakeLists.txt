SET(RESOURCES "")
SET(RESOURCE_PREFIX "")
IF(APPLE)
    SET(RESOURCES
        mac/Info.plist
        mac/cefsimple.icns
        mac/English.lproj/InfoPlist.strings
        mac/English.lproj/MainMenu.xib)
    SET(RESOURCE_PREFIX "mac/")
    APPEND_PLATFORM_SOURCES(RESOURCES)

    ADD_EXECUTABLE(watchercli_Helper MACOSX_BUNDLE cefHelper.cpp)
    SET_TARGET_PROPERTIES(watchercli_Helper PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/helper-Info.plist
        OUTPUT_NAME "watchercli Helper (GPU)"
    )
    ADD_EXECUTABLE(watchercli MACOSX_BUNDLE main.cpp ${RESOURCES})
    SET_TARGET_PROPERTIES(watchercli PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist
    )

ENDIF()
SET_EXECUTABLE_TARGET_PROPERTIES(watchercli)
ADD_DEPENDENCIES(watchercli libcef_dll_wrapper)

TARGET_LINK_LIBRARIES(watchercli messenger sentinel ${Boost_LIBRARIES} libcef_dll_wrapper ${CEF_STANDARD_LIBS})

IF(APPLE)
    SET_EXECUTABLE_TARGET_PROPERTIES(watchercli_Helper)
    ADD_DEPENDENCIES(watchercli_Helper libcef_dll_wrapper)
    TARGET_LINK_LIBRARIES(watchercli_Helper libcef_dll_wrapper ${CEF_STANDARD_LIBS})

    IF(USE_SANDBOX)
        ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
        TARGET_LINK_LIBRARIES(watchercli_Helper cef_sandbox_lib)
    ENDIF()
ENDIF()

TARGET_INCLUDE_DIRECTORIES(watchercli PRIVATE 
    $<TARGET_PROPERTY:sentinel,INTERFACE_INCLUDE_DIRECTORIES>
    ${Boost_INCLUDE_DIRS})
INSTALL_TARGET_APP(watchercli ${RESOURCES} ${RESOURCE_PREFIX})
