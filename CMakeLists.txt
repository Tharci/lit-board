cmake_minimum_required(VERSION 3.17)
project(LitBoard)

set(CMAKE_CXX_STANDARD 17)

link_directories(/usr/lib)

add_executable(LitBoard main.cpp LitBoard.cpp LitBoard.h HIDDevice.cpp HIDDevice.h WeatherData.cpp WeatherData.h credentials.h)

target_link_libraries(LitBoard libhidapi-libusb.so libcurlpp.so libcurl.so libusb-1.0.so)