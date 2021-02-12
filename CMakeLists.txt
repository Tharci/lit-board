cmake_minimum_required(VERSION 3.17)
project(LitBoard)

set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Build type not specified: Use Release by default")
endif (NOT CMAKE_BUILD_TYPE)


include_directories(./external/include)


if (WIN32)
    add_compile_definitions(CURL_STATICLIB)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        link_directories(./external/bin/Release)
        add_link_options(/LTCG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        link_directories(./external/bin/Debug)
    else ()
        message(FATAL_ERROR "Build type must be either \"Debug\" or \"Release\".")
    endif ()


else ()
    link_directories(/usr/lib)
endif ()

add_executable(LitBoard main.cpp LitBoardDriver.cpp LitBoardDriver.h HIDDevice.cpp HIDDevice.h WeatherData.cpp WeatherData.h credentials.h)

if (WIN32)
    target_link_libraries(LitBoard hidapi setupapi libcurl Crypt32 ws2_32 winmm wldap32)
else ()
    target_link_libraries(LitBoard libhidapi-hidraw.so libcurl.so)
endif ()
