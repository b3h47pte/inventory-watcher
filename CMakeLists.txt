CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
PROJECT("inventory-watcher")

SET(CMAKE_DEBUG_POSTFIX "d")
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd")

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

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

MACRO(INSTALL_TARGET_APP TARGET)
    IF(APPLE)
        INSTALL(TARGETS ${TARGET}
                BUNDLE DESTINATION bin)

        SET(${TARGET}_APP_DIR "${CMAKE_INSTALL_PREFIX}/bin/${TARGET}.app")
        ADD_CUSTOM_COMMAND(
            TARGET ${TARGET}
            POST_BUILD
            # Copy the CEF framework into the Frameworks directory.
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
                    "${${TARGET}_APP_DIR}/Contents/Frameworks/Chromium Embedded Framework.framework"
            VERBATIM
        )
    ENDIF()

ENDMACRO()

ADD_SUBDIRECTORY(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
ADD_SUBDIRECTORY("src")
