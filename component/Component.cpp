//
// Created by tmarc on 14/07/2021.
//

#include "Component.h"
#include "LiveWeather.h"

#include <utility>


lbd::comp::Component::Component() = default;

void lbd::comp::Component::keyboardConnected() {
    onKeyboardConnected();
}

void lbd::comp::Component::keyboardDisconnected() {
    onKeyboardDisconnected();
}

void lbd::comp::Component::messageReceived(uint8_t* data, size_t length) {
    onMessageReceived(data, length);
}



