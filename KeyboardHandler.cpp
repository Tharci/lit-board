//
// Created by tmarc on 14/07/2021.
//

#include <iostream>
#include "KeyboardHandler.h"
#include "LitBoardDriver.h"


lbd::KeyboardHandler::KeyboardHandler() : keyboard(0x04d9, 0xa291, 1) {

}

[[noreturn]] void lbd::KeyboardHandler::handleKeyboard() {
    while (true) {
        static bool prevState = false;
        static bool currState;

        currState = keyboard.open();
        if (!currState && prevState) {
            std::cout << "[TRACE] Keyboard has been unplugged.\n\n";

            for(auto& component: LitBoardDriver::getInstance().getComponents()) {
                component.second->onKeyboardConnected();
            }
        }

        if (!prevState && currState) {
            std::cout << "[TRACE] Keyboard has been connected.\n\n";

            for(auto& component: LitBoardDriver::getInstance().getComponents()) {
                component.second->onKeyboardConnected();
            }
        }

        prevState = currState;
        std::flush(std::cout);
        std::this_thread::sleep_for(std::chrono::milliseconds(750));
    }
}
