//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_LIVEWEATHER_H
#define LITBOARD_LIVEWEATHER_H

#include "Component.h"
#include "WeatherData.h"

#include <atomic>


namespace lbd::comp {
    class LiveWeather : public Component {
    public:
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

        [[nodiscard]] ComponentId getComponentId() const override;
        void onKeyboardConnected() override;
        void onKeyboardDisconnected() override;
        WeatherData* getWeatherData();
        Time unixToTime(time_t timeUnix) const;

    private:
        std::atomic<bool> running;
    };
}


#endif //LITBOARD_LIVEWEATHER_H
