//
// Created by tharci on 2/7/21.
//

#include "LitBoardDriver.h"
#include "DriverBehavior.h"
#include "LiveWeatherBehavior.h"

#include <iostream>
#include <vector>
#include <thread>


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace lbd;

void LitBoardDriver::run() {
    std::vector<DriverBehavior*> behaviors {
        new LiveWeatherBehavior(&keyboard)
    };

    for(auto behavior: behaviors) {
        behavior->start();

        if (auto periodicBehavior = dynamic_cast<PeriodicDriverBehavior*>(behavior)) {
            // START PERIODIC TIMER
            std::thread([periodicBehavior]() {
                while (true) {
                    if (periodicBehavior->isActive()) {
                        periodicBehavior->tick();
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(
                        periodicBehavior->getCycleTimeMs()
                    ));
                }
            }).detach();
        }
    }

    std::cout << "[TRACE] LitBoard driver running...\n\n";

    while (true) {
        static bool prevState = false;
        static bool currState;

        currState = keyboard.open();
        if (!currState && prevState) {
            std::cout << "[TRACE] Keyboard has been unplugged.\n\n";
        }

        if (!prevState && currState) {
            std::cout << "[TRACE] Keyboard has been connected.\n\n";

            for(auto behavior: behaviors) {
                behavior->keyboardConnectedCallback();
            }
        }

        prevState = currState;
        std::flush(std::cout);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    for(auto behavior: behaviors) {
        delete behavior;
    }
}

HIDDevice* LitBoardDriver::getKeyboard() {
    return &keyboard;
}

#pragma clang diagnostic pop