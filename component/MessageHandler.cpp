//
// Created by tmarc on 14/07/2021.
//

#include <iostream>
#include "MessageHandler.h"
#include "LitBoardDriver.h"
#include <mutex>


#define BUFFSIZE 512


lbd::comp::MessageHandler::MessageHandler(std::mutex& sleepMutex, std::condition_variable& sleepCondVar)
        : CyclicComponent([this] { asyncTaskCycle(); }, true, sleepMutex, sleepCondVar) { }

lbd::comp::ComponentId lbd::comp::MessageHandler::getComponentId() const {
    return ComponentId::MessageHandler;
}

void lbd::comp::MessageHandler::onKeyboardConnected() {
    notifyKeyboard();
}

void lbd::comp::MessageHandler::send(const lbd::comp::Component &component, const uint8_t *data, size_t length) {
    static const int max_message_length = BUFFSIZE - (sizeof magicalSafetyCode + 4);

    if (length > max_message_length) {
        std::cout << "[ERROR][MESSAGE_HANDLER] Could not send message with length of " << length <<
                     " since it is too large. Max message length is " << max_message_length << std::endl;
        return;
    }

    uint8_t buffer[BUFFSIZE];
    buffer[0] = 0;
    std::memcpy(buffer + 1, magicalSafetyCode, sizeof magicalSafetyCode);
    buffer[sizeof magicalSafetyCode + 1] = (uint8_t) commVersion;
    buffer[sizeof magicalSafetyCode + 2] = (uint8_t) MessageType::ComponentMessage;
    buffer[sizeof magicalSafetyCode + 3] = (uint8_t) component.getComponentId();
    std::memcpy(buffer + sizeof magicalSafetyCode + 4, data, length);
    length += sizeof magicalSafetyCode + 3;

    LitBoardDriver::getInstance().getKeyboardHandler().getKeyboard().write(buffer, length);
}

void lbd::comp::MessageHandler::notifyKeyboard() {
    uint8_t buffer[1 + sizeof magicalSafetyCode + 1 + 1];
    buffer[0] = 0;
    std::memcpy(buffer + 1, magicalSafetyCode, sizeof magicalSafetyCode);
    buffer[sizeof magicalSafetyCode + 1] = (uint8_t) commVersion;
    buffer[sizeof magicalSafetyCode + 2] = (uint8_t) MessageType::DriverConnected;

    LitBoardDriver::getInstance().getKeyboardHandler().getKeyboard().write(buffer, sizeof buffer);
}

void lbd::comp::MessageHandler::asyncTaskCycle() {
    auto& driver = LitBoardDriver::getInstance();
    uint8_t buffer[BUFFSIZE];
    auto result = driver.getKeyboardHandler().getKeyboard().read(buffer, BUFFSIZE);

    if (result > 1) {
        auto componentId = (lbd::comp::ComponentId) buffer[0];
        auto it = driver.getComponents().find(componentId);
        if (it != driver.getComponents().end()) {
            it->second->messageReceived(buffer + 1, result - 1);
        }
    }
    else {
        conditionalSleep(200);
    }
}
