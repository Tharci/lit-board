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

        /**
         * Sends data to the keyboard. Can be used by driver components.
         *
         * Structure of a message:
         * [ PlaceholderByte | magicalSafetyCode | CommVersion | MessageType::ComponentMessage | componentId | message ]
         */
        void send(const Component& component, const uint8_t* data, size_t length);

        void onKeyboardConnected() override;
        void onKeyboardDisconnected() override;

    private:
        /**
         * Magical Safety Code is 4 bytes of safety code to ensure about the source of the messages in both direction.
         */
        const uint8_t magicalSafetyCode[2] = { 0x32, 0xf3 };

        /**
         * Communication Version IDs in order to prevent version mismatch between the driver and the keyboard firmware.
         */
        enum class CommVersion {
            V_0_0_1 = 0,
        };

        /**
         * Current version of the communication used by the driver.
         */
        const CommVersion commVersion = CommVersion::V_0_0_1;

        /**
         * Types of messages the MessageHandler can send.
         */
        enum class MessageType {
            DriverConnected = 1,
            ComponentMessage = 2,
        };

        /**
         * Notifies the keyboard about the driver being connected.
         *
         * Structure of a message:
         * [ PlaceholderByte | magicalSafetyCode | CommVersion | MessageType::DriverConnected ]
         */
        void notifyKeyboard();

        std::atomic<bool> running;
    };
}


#endif //LITBOARD_MESSAGEHANDLER_H
