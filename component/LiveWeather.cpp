//
// Created by tmarc on 14/07/2021.
//

#include "LiveWeather.h"
#include "LitBoardDriver.h"
#include "credentials.h"
#include "json.hpp"

#include <iostream>

#include "curl/curl.h"
#undef min

#ifdef __linux__
#define localtime_s(X, Y) localtime_r(Y, X)
#endif


lbd::comp::LiveWeather::LiveWeather(std::mutex& sleepMutex, std::condition_variable& sleepCondVar) :
        CyclicComponent([this] { asyncTaskCycle(); }, true, sleepMutex, sleepCondVar) { };

lbd::comp::ComponentId lbd::comp::LiveWeather::getComponentId() const {
    return ComponentId::LiveWeather;
}

void lbd::comp::LiveWeather::asyncTaskCycle() {
    auto weatherData = getWeatherData();
    if (weatherData) {
        LitBoardDriver::getInstance().getMessageHandler().send(*this, (uint8_t*) weatherData, sizeof(WeatherData));
        std::cout << "[TRACE][LIVE_WEATHER] Weather data sent.\n";

        conditionalSleep(2 * 60 * 1000);
    }
    else {
        std::cout << "[ERROR][LIVE_WEATHER] Failed to get weather data.\n";
        conditionalSleep(2000);
    }
}


static size_t curlWriteStr(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}


lbd::comp::LiveWeather::WeatherData* lbd::comp::LiveWeather::getWeatherData() {
    static WeatherData weatherData;

    auto curl = curl_easy_init();
    if (!curl) {
        return nullptr;
    }

    const std::string link = "http://api.openweathermap.org/data/2.5/weather?q=" +
            LitBoardDriver::getInstance().getConfigHandler().getConfig().city +
            "&units=metric&appid=" + APIKey;
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteStr);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl = nullptr;

    if (response_code != 0 || response.empty()) {
        return nullptr;
    }

    std::cout << "\n[TRACE][LIVE_WEATHER] HTML Response: \n" << response << "\n\n";

    auto js = nlohmann::json::parse(response);

    if (js["cod"] == 200) {
        weatherData = {};

        auto jsMain = js["main"];

        weatherData.temp = jsMain["temp"];
        weatherData.tempMin = jsMain["temp_min"];
        weatherData.tempMax = jsMain["temp_max"];


        // weatherData.time = unixToTime((time_t)js["dt"]);
        weatherData.time = unixToTime((time_t)time(nullptr));
        weatherData.sunriseTime = unixToTime((time_t)js["sys"]["sunrise"]);
        weatherData.sunsetTime = unixToTime((time_t)js["sys"]["sunset"]);


        if (js.contains("wind")) {
            weatherData.windIntensity = std::min((int)UINT8_MAX, (int)js["wind"]["speed"]);
        }

        if (js.contains("clouds")) {
            weatherData.cloudDensity = js["clouds"]["all"];
            weatherData.sunIntensity = 100 - weatherData.cloudDensity;
        }

        if (js.contains("rain")) {
            // 25mm+ counts as 100% intensity.
            int rainInt = std::min((int)UINT8_MAX, (int)ceil(js["rain"]["1h"].get<double>())) * 4;
            if (rainInt > 100)
                rainInt = 100;
            weatherData.rainIntensity = rainInt;
        }

        if (js.contains("snow")) {
            // 25mm+ counts as 100% intensity.
            int snowInt = std::min((int)UINT8_MAX, (int)ceil(js["snow"]["1h"].get<double>())) * 4;
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


        return &weatherData;
    }
    else {
        std::cout << "[ERROR][LIVE_WEATHER] Unexpected HTML Response:\n "
                  << js["message"].get<std::string>() << std::endl;
    }

    return nullptr;
}

lbd::comp::LiveWeather::Time lbd::comp::LiveWeather::unixToTime(time_t timeUnix) {
    tm timeStruct {};
    localtime_s(&timeStruct, &timeUnix);

    return {
        (uint8_t) timeStruct.tm_hour,
        (uint8_t) timeStruct.tm_min,
        (uint8_t) timeStruct.tm_sec
    };
}


/*
response = R"""(
{
    "coord":{"lon":19.0756,"lat":47.6694},
    "weather":[{"id":500,"main":"Rain","description":"sunny","icon":"10n"}],
    "base":"stations",
    "main": {
        "temp":-45,"feels_like":-5.27,
        "temp_min":1.11,
        "temp_max":1.67,
        "pressure":997,
        "humidity":88
    },
    "visibility":10000,
    "wind":{"speed":6.71,"deg":265,"gust":9.39},
    "rain":{"1h":0},
    "clouds":{"all":0},
    "dt":1612764070,
    "sys":{"type":3,"id":2009661,
        "country":"HU",
        "sunrise":1612764045,
        "sunset":1612799706
    },
    "timezone":3600,"id":3044681,"name":"Szentendre","cod":200
}
)""";
*/

