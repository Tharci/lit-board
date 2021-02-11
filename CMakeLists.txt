cmake_minimum_required(VERSION 3.17)
project(LitBoard)

set(CMAKE_CXX_STANDARD 17)

link_directories(/usr/lib)

add_executable(LitBoard main.cpp LitBoardDriver.cpp LitBoardDriver.h HIDDevice.cpp HIDDevice.h WeatherData.cpp WeatherData.h credentials.h Logger.cpp Logger.h)

#target_link_libraries(LitBoard libhidapi-libusb.so libcurlpp.so libcurl.so)

target_link_libraries(LitBoard libhidapi-hidraw.so libcurlpp.so libcurl.so)