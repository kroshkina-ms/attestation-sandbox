cmake_minimum_required(VERSION 3.6.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(maa-validate-jwt)

if(CMAKE_HOST_WIN32)
    set(WINDOWS 1)
    set(VCPKG_PATH "C:/src/vcpkg/installed/x64-windows") # TODO: combine this steps with the build script
    #set(ZLIB_ROOT ${VCPKG_PATH})
    #set(CURL_DIR ${VCPKG_PATH}/share/curl)
    find_package(ZLIB REQUIRED)
    find_package(CURL CONFIG REQUIRED)
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
    find_package(CURL REQUIRED)
endif()


set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/options.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
set(HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/ascii_escape_code.hpp ${CMAKE_CURRENT_SOURCE_DIR}/include/options.hpp ${CMAKE_CURRENT_SOURCE_DIR}/include/base64.hpp)

add_executable(maa-validate-jwt ${SOURCES} ${HEADERS})

target_link_libraries(maa-validate-jwt PRIVATE CURL::libcurl)
target_include_directories(maa-validate-jwt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WINDOWS)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT maa-validate-jwt)
#elseif(LINUX)
endif()

