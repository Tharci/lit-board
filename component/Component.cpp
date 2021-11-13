//
// Created by tmarc on 14/07/2021.
//

#include "Component.h"
#include "LiveWeather.h"

#include <utility>


lbd::comp::Component::Component() {

}

lbd::comp::Component::Component(std::function<void()> asyncTaskCycle, bool running)
        : asyncTaskCycle(std::move(asyncTaskCycle)), taskThread(std::async(std::launch::async, [] {})),
          running(running) { }

void lbd::comp::Component::keyboardConnected() {
    connected = true;
    launchAsyncCyclicTask();
    onKeyboardConnected();
}

void lbd::comp::Component::keyboardDisconnected() {
    connected = false;
    onKeyboardDisconnected();
}

void lbd::comp::Component::messageReceived(uint8_t* data, size_t length) {
    onMessageReceived(data, length);
}

void lbd::comp::Component::startAsyncCyclicTask() {
    running = true;
    launchAsyncCyclicTask();
}

void lbd::comp::Component::stopAsyncCyclicTask() {
    running = false;
}

void lbd::comp::Component::launchAsyncCyclicTask() {
    // Do not launch if the asyncTask is still connected
    // Never true, if the called constructor was not the one with asyncTaskCycle,
    //      because taskThread does not get initialized
    if (connected && running && taskThread._Is_ready()) {
        taskThread = std::async(std::launch::async, [&] {
            while (connected && running) {
                asyncTaskCycle();
            }
        });
    }
}

void lbd::comp::Component::cycleWait(long long int millisec) {
    const unsigned step = 400;

    while (millisec > step && connected && running) {
        auto millisecToSleep = millisec < step ? millisec : 200;
        std::this_thread::sleep_for(std::chrono::milliseconds(millisecToSleep));
        millisec -= millisecToSleep;
    }

    if (millisec && connected && running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
    }
}



