//
// Created by tharci on 2/7/21.
//

#include "LitBoard.h"

#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>

#include <vector>
#include <hidapi/hidapi.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include <chrono>
#include <time.h>
#include "external/json.hpp"



/*
 *
NOTIF ->

 if SHANPCHAT -> HTML(litboard.tharci.fail, "notif, snap")

 readData(String data) {
    if (notif) -> if (snap) ->


    mqtt.subscribe("/litboard/notif", foo)

    mqtt.send("/litboard/notif", "notif, snap")

   JAVA ---> (DATA) ---> MQTT ---> (DATA) ---> driver
 }
 */


using namespace lbd;

LitBoardDriver* LitBoardDriver::instance = nullptr;


LitBoardDriver* LitBoardDriver::getInstance() {
    return instance ? instance : new LitBoardDriver;
}


[[noreturn]] void LitBoardDriver::run() {
    while (true) {
        if(keyboard == nullptr || !keyboard->isOpen()) {
            delete keyboard;

            keyboard = getKeyboard();

            if (keyboard) {
                keyboard->open();
                std::cout << "[TRACE] Keyboard has been found.\n\n";
            }
            else {
                std::cout << "[TRACE] Keyboard not found.\n\n";
            }
            std::flush(std::cout);
        }

        if (keyboard) {
            auto result = sendWeatherData();
            if (result < 0) {
                std::cout << "[TRACE] Failed to weather data.\n\n";
            }
            else {
                std::cout << "[TRACE] Weather data successfully sent.\n\n";
            }
            std::flush(std::cout);
            sleep(300);
        }
        else {
            sleep(2);
        }
    }

    // keyboard->close();
}

int LitBoardDriver::sendWeatherData() {
    const std::string APIKey = "50292e2d1fdb13cf5379bca1bf0b2f09";
    const std::string city = "Szentendre";

    curlpp::Cleanup cleanup;

    std::ostringstream os;
    os << curlpp::options::Url(std::string("http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=metric&appid=" + APIKey));

    std::string response = os.str();
    std::cout << "HTML Response: \n" << response << std::endl;

    auto js = nlohmann::json::parse(response);
    if (js["cod"] == 200) {
        WeatherData weatherData;

        auto jsMain = js["main"];

        weatherData.temp = jsMain["temp"];
        weatherData.tempMin = jsMain["temp_min"];
        weatherData.tempMax = jsMain["temp_max"];

        int tzone = js["timezone"];
        weatherData.time = unixToTime((time_t)js["dt"] + tzone);
        weatherData.sunriseTime = unixToTime((time_t)js["sys"]["sunrise"] + tzone);
        weatherData.sunsetTime = unixToTime((time_t)js["sys"]["sunset"] + tzone);


        if (js.contains("wind")) {
            weatherData.windIntensity = std::min(UINT8_MAX, (int)js["wind"]["speed"]);
        }

        if (js.contains("clouds")) {
            weatherData.cloudDensity = js["clouds"]["all"];
            weatherData.sunIntensity = 100 - weatherData.cloudDensity;
        }

        if (js.contains("rain")) {
            weatherData.rainIntensity = std::min(UINT8_MAX, (int)(js["rain"]["1h"]));
        }

        if (js.contains("snow")) {
            weatherData.snowIntensity = std::min(UINT8_MAX, (int)js["snow"]["1h"]);
        }

        if (js["weather"][0]["main"] == "mist") {
            weatherData.mist = true;
        }


        std::string weatherDesc = js["weather"][0]["description"];
        if (weatherDesc.find("light thunderstorm") != std::string::npos) {
            weatherData.stormIntensity = 25;
        }
        else if (weatherDesc.find("heavy thunderstorm") != std::string::npos) {
            weatherData.stormIntensity = 75;
        }
        else if (weatherDesc.find("ragged thunderstorm") != std::string::npos) {
            weatherData.stormIntensity = 100;
        }
        else if (weatherDesc.find("thunderstorm") != std::string::npos) {
            weatherData.stormIntensity = 50;
        }
        else if (weatherDesc.find("thunderstorm") != std::string::npos) {
            weatherData.stormIntensity = 50;
        }


        const auto dataSize = 2 + sizeof(WeatherData);
        unsigned char data[64];

        data[0] = 0;
        data[1] = (unsigned char) MessageType::Weather;
        std::memcpy(data + 2, (unsigned char*) (&weatherData), sizeof(WeatherData));

        std::cout << "Data package size: " << dataSize << std::endl;
        return keyboard->write(data, dataSize);
    }

    return -1;


    // wIdx = (wIdx + 1) % weathers.size();
}

LitBoardDriver::~LitBoardDriver() {
    delete keyboard;
}


HIDDevice* LitBoardDriver::getKeyboard() {
    const unsigned short vid = 0x04d9;
    const unsigned short pid = 0xa291;

    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev) {
        printf("[TRACE] Device: vid/pid: %04hx/%04hx\n  path: %s\n  serial_number: %ls usage_page: %x, usage: %x",
               cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number, cur_dev->usage_page, cur_dev->usage);
        printf("\n");
        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printf("  Product:      %ls\n", cur_dev->product_string);
        printf("  Interface:    %d\n", cur_dev->interface_number);
        printf("\n");
        if( cur_dev->vendor_id == vid && cur_dev->product_id == pid && cur_dev->interface_number == 1) {
            return new HIDDevice(cur_dev);
        }

        cur_dev = cur_dev->next;
    }

    hid_free_enumeration(devs);

    return nullptr;
}

Time LitBoardDriver::unixToTime(time_t timeUnix) {
    tm ttm{};
    time_t t = time(&timeUnix);
    ttm = * localtime(&t);

    return {
        (uint8_t) ttm.tm_hour,
        (uint8_t) ttm.tm_min,
        (uint8_t) ttm.tm_sec
    };
}


