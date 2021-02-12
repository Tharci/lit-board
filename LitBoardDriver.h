//
// Created by tharci on 2/7/21.
//

#ifndef LITBOARD_LITBOARDDRIVER_H
#define LITBOARD_LITBOARDDRIVER_H


#include <vector>
#include <iostream>

#include "HIDDevice.h"
#include "WeatherData.h"

namespace lbd {
    class LitBoardDriver {
    public:
        enum class MessageType {
            Weather = 0x11, Gaming, Notification
        };

        static LitBoardDriver* getInstance();
        static Time unixToTime(time_t time);

        [[noreturn]] void run();
        int sendWeatherData();

    private:
        static LitBoardDriver* instance;
        HIDDevice keyboard;
    };
}





#endif //LITBOARD_LITBOARDDRIVER_H
