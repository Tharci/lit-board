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
#include <libusb-1.0/libusb.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include <chrono>
#include <ctime>
#include "external/json.hpp"

#include "credentials.h"


using namespace lbd;

LitBoardDriver* LitBoardDriver::instance = nullptr;


LitBoardDriver* LitBoardDriver::getInstance() {
    return instance ? instance : new LitBoardDriver;
}


void LitBoardDriver::run() {
    std::cout << "[TRACE] LitBoard driver running...\n\n";
    time_t weatherLastSent = 0;

    while (true) {
        time_t currTime = time(nullptr);

        static bool prevState = false;
        static bool currState;

        currState = keyboard.open();
        if (!currState && prevState) {
            std::cout << "[TRACE] Keyboard has been unplugged.\n\n";
        }

        if (!prevState && currState) {
            std::cout << "[TRACE] Keyboard has been connected.\n\n";
            weatherLastSent = 0;
        }

        static bool initialSendSuccess = false;

        if (!initialSendSuccess || currTime - weatherLastSent >= 300) {
            auto result = sendWeatherData();
            if (result < 0) {
                std::cout << "[TRACE] Failed to send weather data.\n\n";
            }
            else {
                std::cout << "[TRACE] Weather data successfully sent.\n\n";
                initialSendSuccess = true;
            }

            weatherLastSent = currTime;
        }

        prevState = currState;
        std::flush(std::cout);
        usleep(2000000);
    }
}

int LitBoardDriver::sendWeatherData() {
    const std::string city = "Szentendre";

    curlpp::Cleanup cleanup;

    std::ostringstream os;
    try {
        os << curlpp::options::Url(std::string("http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=metric&appid=" + APIKey));
    } catch (...) {
        return -1;
    }

 /*
    std::string response = R"""(
{"coord":{"lon":19.0756,"lat":47.6694},"weather":[{"id":500,"main":"Rain","description":"light thunderstorm","icon":"10n"}],"base":"stations","main":{"temp":1.46,"feels_like":-5.27,"temp_min":1.11,"temp_max":1.67,"pressure":997,"humidity":88},"visibility":10000,"wind":{"speed":6.71,"deg":265,"gust":9.39},"rain":{"1h":0.4},"clouds":{"all":99},"dt":1612808124,"sys":{"type":3,"id":2009661,"country":"HU","sunrise":1612764045,"sunset":1612799706},"timezone":3600,"id":3044681,"name":"Szentendre","cod":200}
)""";
 */
    std::string response = os.str();

    std::cout << "[TRACE] HTML Response: \n" << response << "\n\n";

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
            // 25mm+ counts as 100% intensity.
            int rainInt = std::min(UINT8_MAX, (int)ceil(js["rain"]["1h"].get<double>())) * 4;
            if (rainInt > 100)
                rainInt = 100;
            weatherData.rainIntensity = rainInt;
        }

        if (js.contains("snow")) {
            // 25mm+ counts as 100% intensity.
            int snowInt = std::min(UINT8_MAX, (int)ceil(js["snow"]["1h"].get<double>())) * 4;
            if (snowInt > 100)
                snowInt = 100;
            weatherData.snowIntensity = snowInt;
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


        const auto dataSize = 2 + sizeof(WeatherData);
        unsigned char data[64];

        data[0] = 0;
        data[1] = (unsigned char) MessageType::Weather;
        std::memcpy(data + 2, (unsigned char*) (&weatherData), sizeof(WeatherData));

        return keyboard.write(data, dataSize);
    }
    else if (js["cod"] == "404") {
        std::cout << "[ERROR] " << js["message"].get<std::string>() << std::endl;
    }

    return -1;


    // wIdx = (wIdx + 1) % weathers.size();
}


Time LitBoardDriver::unixToTime(time_t timeUnix) {
    tm ttm = *localtime(&timeUnix);

    return {
        (uint8_t) ttm.tm_hour,
        (uint8_t) ttm.tm_min,
        (uint8_t) ttm.tm_sec
    };
}


