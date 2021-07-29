//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_MESSAGEHANDLER_H
#define LITBOARD_MESSAGEHANDLER_H

#include "Component.h"
#include "HIDDevice.h"

#include <cstdint>
#include <thread>
#include <atomic>


namespace lbd::comp {
    class MessageHandler : public Component {
    public:
        [[nodiscard]] ComponentId getComponentId() const override;
        void send(const Component& component, const uint8_t* data, size_t length);
        void onKeyboardConnected() override;
        void onKeyboardDisconnected() override;

    private:
        enum class MessageType {
            DriverConnected = 1,
            ComponentMessage = 2,
        };
        void notifyKeyboard();

        std::atomic<bool> running;
        const uint8_t litBoardMagicCode[2] = { 0x32, 0xf3 };
    };
}


#endif //LITBOARD_MESSAGEHANDLER_H
