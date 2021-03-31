//
// Created by tharci on 2/15/21.
//

#include "LiveWeatherBehavior.h"
#include "MessageType.h"

#include "credentials.h"
#include "json.hpp"

#include "curl/curl.h"
#undef min

#ifdef __linux__
#define localtime_s(X, Y) localtime_r(Y, X)
#endif


void lbd::LiveWeatherBehavior::tick() {
    sendWeatherData(getWeatherData());
}


static size_t curlWriteStr(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}


WeatherData* lbd::LiveWeatherBehavior::getWeatherData() {
    static WeatherData weatherData;

    auto curl = curl_easy_init();
    if (!curl) {
        return nullptr;
    }

    const std::string city = "Szentendre";
    const std::string link = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=metric&appid=" + APIKey;
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


    std::cout << "[TRACE] HTML Response: \n" << response << "\n\n";

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
    else if (js["cod"] == "404") {
        std::cout << "[ERROR] " << js["message"].get<std::string>() << std::endl;
    }
    else {

    }

    return nullptr;

}

bool lbd::LiveWeatherBehavior::sendWeatherData(WeatherData* weatherData) {
    if (weatherData == nullptr) {
        return false;
    }

    const auto dataSize = 2 + sizeof(WeatherData);
    unsigned char data[64];

    data[0] = 0;
    data[1] = (unsigned char) MessageType::Weather;
    std::memcpy(data + 2, (unsigned char*) (weatherData), sizeof(WeatherData));

    return keyboard->write(data, dataSize);
}

Time lbd::LiveWeatherBehavior::unixToTime(time_t timeUnix) {
    tm timeStruct {};
    localtime_s(&timeStruct, &timeUnix);

    return {
            (uint8_t) timeStruct.tm_hour,
            (uint8_t) timeStruct.tm_min,
            (uint8_t) timeStruct.tm_sec
    };
}

void lbd::LiveWeatherBehavior::keyboardConnectedCallback() {
    tick();
}

