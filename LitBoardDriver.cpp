//
// Created by tmarc on 14/07/2021.
//

#include <iostream>
#include "LitBoardDriver.h"

lbd::LitBoardDriver& lbd::LitBoardDriver::getInstance() {
    static LitBoardDriver litBoardDriver;
    return litBoardDriver;
}

void lbd::LitBoardDriver::run() {
    configHandler.load();
    if (configHandler.isLoaded()) {
        std::cout << "Config successfully loaded.\n";
        keyboardHandler.handleKeyboard();
    }
    else {
        std::cout << "Config could not be loaded. Exiting...\n";
    }
}

lbd::ConfigHandler& lbd::LitBoardDriver::getConfigHandler() {
    return configHandler;
}

lbd::KeyboardHandler& lbd::LitBoardDriver::getKeyboardHandler() {
    return keyboardHandler;
}

lbd::comp::MessageHandler& lbd::LitBoardDriver::getMessageHandler() {
    return messageHandler;
}

std::unordered_map<lbd::comp::ComponentId, lbd::comp::Component*>& lbd::LitBoardDriver::getComponents() {
    return components;
}

void lbd::LitBoardDriver::addComponent(lbd::comp::Component *component) {
    components.emplace(component->getComponentId(), component);
}

lbd::LitBoardDriver::LitBoardDriver() : keyboardHandler(sleepMutex, sleepCondVar) {
    addComponent(&messageHandler);
    addComponent(&liveWeather);
    addComponent(&appIntegration);
    addComponent(&audioVisualizer);
    addComponent(&notificationHandler);
    addComponent(&serverHandler);
    addComponent(&stateHandler);
}
