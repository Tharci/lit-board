//
// Created by tmarc on 14/07/2021.
//

#include <iostream>
#include "MessageHandler.h"
#include "LitBoardDriver.h"
#include "Component.h"


#define BUFFSIZE 512

lbd::comp::ComponentId lbd::comp::MessageHandler::getComponentId() const {
    return ComponentId::MessageHandler;
}

void lbd::comp::MessageHandler::onKeyboardConnected() {
    running = true;

    std::thread([&] {
        auto& driver = LitBoardDriver::getInstance();

        notifyKeyboard();

        while (running) {
            uint8_t buffer[BUFFSIZE];
            auto result = driver.getKeyboardHandler().getKeyboard().read(buffer, BUFFSIZE);
            auto componentId = (lbd::comp::ComponentId) buffer[0];

            if (result > 1) {
                auto it = driver.getComponents().find(componentId);
                if (it != driver.getComponents().end()) {
                    it->second->onMessageReceived(buffer + 1, result - 1);
                }
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
    }).detach();
}

void lbd::comp::MessageHandler::onKeyboardDisconnected() {
    running = false;
}

/*
 * Structure of a message:
 * [ PlaceholderByte | litBoardMagicCode | MessageType::ComponentMessage | componentId | message ]
 */
void lbd::comp::MessageHandler::send(const lbd::comp::Component &component, const uint8_t *data, size_t length) {
    uint8_t buffer[BUFFSIZE];
    buffer[0] = 0;
    std::memcpy(buffer + 1, litBoardMagicCode, sizeof litBoardMagicCode);
    buffer[sizeof litBoardMagicCode + 1] = (uint8_t) component.getComponentId();
    std::memcpy(buffer + sizeof litBoardMagicCode + 2, data, length);
    length += sizeof litBoardMagicCode + 2;

    LitBoardDriver::getInstance().getKeyboardHandler().getKeyboard().write(buffer, length);
}

/*
 * Structure of a message:
 * [ PlaceholderByte | litBoardMagicCode | MessageType::DriverConnected ]
 */
void lbd::comp::MessageHandler::notifyKeyboard() {
    uint8_t buffer[1 + sizeof litBoardMagicCode + 1];
    buffer[0] = 0;
    std::memcpy(buffer + 1, litBoardMagicCode, sizeof litBoardMagicCode);
    buffer[sizeof litBoardMagicCode + 1] = (uint8_t) MessageType::DriverConnected;

    LitBoardDriver::getInstance().getKeyboardHandler().getKeyboard().write(buffer, sizeof buffer);
}
