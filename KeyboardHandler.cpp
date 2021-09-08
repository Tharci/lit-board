//
// Created by tmarc on 14/07/2021.
//

#include <iostream>
#include "KeyboardHandler.h"
#include "LitBoardDriver.h"


lbd::KeyboardHandler::KeyboardHandler() : keyboard(0x04d9, 0xa291, 1) {

}

void lbd::KeyboardHandler::handleKeyboard() {
    bool prevState = false;
    bool currState;

    while (true) {
        currState = keyboard.open();
        if (!currState && prevState) {
            std::cout << "[TRACE] Keyboard has been unplugged.\n";

            for(auto& component: LitBoardDriver::getInstance().getComponents()) {
                component.second->onKeyboardDisconnected();
            }
        }

        if (!prevState && currState) {
            std::cout << "[TRACE] Keyboard has been connected.\n";

            for(auto& component: LitBoardDriver::getInstance().getComponents()) {
                component.second->onKeyboardConnected();
            }
        }

        prevState = currState;
        std::flush(std::cout);
        std::this_thread::sleep_for(std::chrono::milliseconds(750));
    }
}

bool lbd::KeyboardHandler::isConnected() {
    return keyboard.isOpen();
}

lbd::HIDDevice& lbd::KeyboardHandler::getKeyboard() {
    return keyboard;
}
