//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_COMPONENT_H
#define LITBOARD_COMPONENT_H

#include <cstdint>



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
        [[nodiscard]] virtual ComponentId getComponentId() const = 0;

        /*
         * Event functions can be implemented in the child classes.
         *      These are called when the given event occurs.
         */
        virtual void onKeyboardConnected() {}
        virtual void onKeyboardDisconnected() {}
        virtual void onMessageReceived(uint8_t* data, size_t length) {}

    private:
    };
}


#endif //LITBOARD_COMPONENT_H
