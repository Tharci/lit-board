//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_COMPONENT_H
#define LITBOARD_COMPONENT_H

#include <cstdint>
#include <functional>
#include <future>


namespace lbd::comp {
    enum class ComponentId {
        AppIntegration      = 1,
        AudioVisualizer     = 2,
        LiveWeather         = 3,
        MessageHandler      = 4,
        NotificationHandler = 5,
        ServerHandler       = 6,
        StateHandler        = 7
    };

    class Component {
    public:
        Component();
        virtual void keyboardConnected();
        virtual void keyboardDisconnected();
        void messageReceived(uint8_t* data, size_t length);

        [[nodiscard]] virtual ComponentId getComponentId() const = 0;

    protected:
        /*
         * Event functions can be implemented in the child classes.
         *      These are called when the given event occurs.
         */
        virtual void onKeyboardConnected() {}
        virtual void onKeyboardDisconnected() {}
        virtual void onMessageReceived(uint8_t* data, size_t length) {}
    };
}


#endif //LITBOARD_COMPONENT_H
