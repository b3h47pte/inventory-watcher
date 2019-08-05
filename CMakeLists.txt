CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
PROJECT("inventory-watcher")

SET(CMAKE_DEBUG_POSTFIX "d")
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd")

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/out")

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Werror")
SET(CMAKE_CXX_STANDARD 17)

IF(APPLE)
    SET(CEF_ROOT "${CMAKE_SOURCE_DIR}/deps/cef/mac")
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/deps/cef/mac/cmake")
ENDIF()

# For YCM
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
FIND_PACKAGE(Boost 1.70
             COMPONENTS program_options
             REQUIRED)
FIND_PACKAGE(CURL REQUIRED)
FIND_PACKAGE(openssl REQUIRED)

FIND_PACKAGE(CEF REQUIRED)
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(Gumbo REQUIRED IMPORTED_TARGET gumbo)

MACRO(INTERNAL_CEF_CREATE_HELPERS TARGET SRC PLIST TARGET_SUFFIX SUFFIX)
    ADD_EXECUTABLE(${TARGET}_Helper${TARGET_SUFFIX} MACOSX_BUNDLE ${SRC})
    SET_TARGET_PROPERTIES(${TARGET}_Helper${TARGET_SUFFIX} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${PLIST}
        OUTPUT_NAME "${TARGET} Helper${SUFFIX}"
    )

    SET_EXECUTABLE_TARGET_PROPERTIES(${TARGET}_Helper${TARGET_SUFFIX})
    ADD_DEPENDENCIES(${TARGET}_Helper${TARGET_SUFFIX} libcef_dll_wrapper)
    TARGET_LINK_LIBRARIES(${TARGET}_Helper${TARGET_SUFFIX} libcef_dll_wrapper ${CEF_STANDARD_LIBS})

    IF(USE_SANDBOX)
        TARGET_LINK_LIBRARIES(${TARGET}_Helper${TARGET_SUFFIX} cef_sandbox_lib)
    ENDIF()
ENDMACRO()

MACRO(CEF_CREATE_HELPERS TARGET SRC PLIST)
    INTERNAL_CEF_CREATE_HELPERS(${TARGET} ${SRC} ${PLIST} "" "")
    INTERNAL_CEF_CREATE_HELPERS(${TARGET} ${SRC} ${PLIST} "_GPU" " (GPU)")
    INTERNAL_CEF_CREATE_HELPERS(${TARGET} ${SRC} ${PLIST} "_Renderer" " (Renderer)")
ENDMACRO()

MACRO(INSTALL_TARGET_APP TARGET RESOURCES RESOURCE_PREFIX)
    IF(APPLE)
        INSTALL(TARGETS ${TARGET}
                BUNDLE DESTINATION bin)
        SET(${TARGET}_APP_DIR "${CMAKE_INSTALL_PREFIX}/bin/${TARGET}.app")

        ADD_CUSTOM_TARGET(${TARGET}_PROXY ALL)
        ADD_CUSTOM_COMMAND(
            TARGET ${TARGET}_PROXY
            POST_BUILD
            # Copy the helper app bundle into the Frameworks directory.
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET} Helper.app"
                    "${${TARGET}_APP_DIR}/Contents/Frameworks/${TARGET} Helper.app"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET} Helper (GPU).app"
                    "${${TARGET}_APP_DIR}/Contents/Frameworks/${TARGET} Helper (GPU).app"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET} Helper (Renderer).app"
                    "${${TARGET}_APP_DIR}/Contents/Frameworks/${TARGET} Helper (Renderer).app"
            # Copy the CEF framework into the Frameworks directory.
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
                    "${${TARGET}_APP_DIR}/Contents/Frameworks/Chromium Embedded Framework.framework"
            VERBATIM
        )

        COPY_MACOSX_RESOURCES("${RESOURCES}" "${RESOURCE_PREFIX}" "${TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${${TARGET}_APP_DIR}")

    ENDIF()
ENDMACRO()

ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
ADD_SUBDIRECTORY(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
ADD_SUBDIRECTORY("src")
