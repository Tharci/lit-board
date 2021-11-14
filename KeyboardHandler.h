//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_KEYBOARDHANDLER_H
#define LITBOARD_KEYBOARDHANDLER_H

#include "HIDDevice.h"

#include <mutex>


namespace lbd {
    class KeyboardHandler {
    public:
        KeyboardHandler(std::mutex& sleepMutex, std::condition_variable& sleepCondVar);

        [[noreturn]] void handleKeyboard();
        [[nodiscard]] bool isConnected();
        [[nodiscard]] HIDDevice& getKeyboard();

    private:
        HIDDevice keyboard;

        std::mutex& sleepMutex;
        std::condition_variable& sleepCondVar;
    };
}


#endif //LITBOARD_KEYBOARDHANDLER_H
