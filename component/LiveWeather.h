//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_LIVEWEATHER_H
#define LITBOARD_LIVEWEATHER_H

#include "Component.h"
#include "WeatherData.h"

#include <atomic>
#include <thread>
#include <future>


namespace lbd::comp {
    class LiveWeather : public Component {
    public:
        struct Time {
            uint8_t hour   = 0;
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

        LiveWeather();
        [[nodiscard]] ComponentId getComponentId() const override;
        static WeatherData* getWeatherData();
        static Time unixToTime(time_t timeUnix) ;

    private:
        void asyncTaskCycle();
    };
}


#endif //LITBOARD_LIVEWEATHER_H
