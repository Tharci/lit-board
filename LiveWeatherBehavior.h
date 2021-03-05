//
// Created by tharci on 2/15/21.
//

#ifndef LITBOARD_LIVEWEATHERBEHAVIOR_H
#define LITBOARD_LIVEWEATHERBEHAVIOR_H


#include "PeriodicDriverBehavior.h"
#include "HIDDevice.h"
#include "WeatherData.h"

namespace lbd {
    class LiveWeatherBehavior: public PeriodicDriverBehavior {
    public:
        LiveWeatherBehavior(HIDDevice* keyboard) : PeriodicDriverBehavior(keyboard, 300000) {}

        void tick() override;
        void keyboardConnectedCallback() override;

    private:
        static WeatherData* getWeatherData();
        bool sendWeatherData(WeatherData* weatherData);
        static Time unixToTime(time_t timeUnix);
    };
}


#endif //LITBOARD_LIVEWEATHERBEHAVIOR_H
