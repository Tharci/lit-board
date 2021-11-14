//
// Created by tmarc on 14/11/2021.
//

#include "CyclicComponent.h"


void lbd::comp::CyclicComponent::startAsyncCyclicTask() {
    running = true;
    launchAsyncCyclicTask();
}

void lbd::comp::CyclicComponent::stopAsyncCyclicTask() {
    running = false;
}

void lbd::comp::CyclicComponent::keyboardConnected() {
    connected = true;
    launchAsyncCyclicTask();
    Component::keyboardConnected();
}

void lbd::comp::CyclicComponent::keyboardDisconnected() {
    connected = false;
    Component::keyboardDisconnected();
}

void lbd::comp::CyclicComponent::launchAsyncCyclicTask() {
    // Do not launch if the asyncTask is still connected
    // Never true, if the called constructor was not the one with asyncTaskCycle,
    //      because taskThread does not get initialized
    if (connected && running && taskThread._Is_ready()) {
        taskThread = std::async(std::launch::async, [&] {
            while (connected && running) {
                asyncTaskCycle();
            }
        });
    }
}

void lbd::comp::CyclicComponent::conditionalSleep(long long int durationMs) {
    std::unique_lock lock(sleepMutex);
    if (running && connected) {
        sleepCondVar.wait_for(lock, std::chrono::milliseconds(durationMs),
                              [&] { return !(running && connected); });
    }
}


