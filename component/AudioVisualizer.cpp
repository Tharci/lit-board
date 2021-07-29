//
// Created by tmarc on 14/07/2021.
//

#include <iostream>
#include <thread>
#include <LitBoardDriver.h>
#include "AudioVisualizer.h"

#define AUDIOVIS_START 1
#define AUDIOVIS_STOP 0

lbd::comp::ComponentId lbd::comp::AudioVisualizer::getComponentId() const {
    return ComponentId::AudioVisualizer;
}

void lbd::comp::AudioVisualizer::onMessageReceived(uint8_t *data, size_t length) {
    if (length != 1) {
        std::cout << "[ERROR][AUDIOVISUALIZER] Unexpected message length of '" << length << "' received. Expected length of 1.\n";
        return;
    }

    switch (data[0]) {
        case AUDIOVIS_START:
            start();
            break;
        case AUDIOVIS_STOP:
            stop();
            break;
        default:
            std::cout << "[ERROR][AUDIOVISUALIZER] Unexpected message '" << data[0] << "' received. Expected '0' or '1'.\n";
    }
}

void lbd::comp::AudioVisualizer::start() {
    if (running)
        return;

    running = true;
    std::thread([&] {
        auto& driver = LitBoardDriver::getInstance();

        while (running) {
            // send audio data

            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 20 fps
        }
    }).detach();
}

void lbd::comp::AudioVisualizer::stop() {
    running = false;
}
