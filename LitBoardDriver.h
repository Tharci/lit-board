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
        HIDDevice* getKeyboard();
        [[noreturn]] void run();

    private:
        HIDDevice keyboard;
    };
}





#endif //LITBOARD_LITBOARDDRIVER_H
