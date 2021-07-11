//
// Created by tharci on 2/15/21.
//

#ifndef LITBOARD_LIVEWEATHERBEHAVIOR_H
#define LITBOARD_LIVEWEATHERBEHAVIOR_H


#include "PeriodicDriverBehavior.h"
#include "HIDDevice.h"
#include "WeatherData.h"
#include <iostream>
#include <fstream>

namespace lbd {
    class LiveWeatherBehavior: public PeriodicDriverBehavior {
    public:
        explicit LiveWeatherBehavior(HIDDevice* keyboard);

        void tick() override;
        void keyboardConnectedCallback() override;

    private:
        std::string city;
        WeatherData* getWeatherData();
        bool sendWeatherData(WeatherData* weatherData);
        static Time unixToTime(time_t timeUnix);
        void loadConfig();
    };
}


#endif //LITBOARD_LIVEWEATHERBEHAVIOR_H
