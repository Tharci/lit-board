//
// Created by tharci on 2/8/21.
//

#ifndef LITBOARD_WEATHERDATA_H
#define LITBOARD_WEATHERDATA_H


#include <cstdint>

struct Time {
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
};

struct WeatherData {
    Time time;
    Time sunriseTime;
    Time sunsetTime;
    int8_t temp = 0;
    int8_t tempMin = 0;
    int8_t tempMax = 0;
    uint8_t sunIntensity = 0;
    uint8_t cloudDensity = 0;
    uint8_t windIntensity = 0;
    uint8_t rainIntensity = 0;
    uint8_t stormIntensity = 0;
    uint8_t snowIntensity = 0;
    bool mist = false;
};


#endif //LITBOARD_WEATHERDATA_H
