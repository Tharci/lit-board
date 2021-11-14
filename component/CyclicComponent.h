//
// Created by tmarc on 14/11/2021.
//

#ifndef LITBOARD_CYCLICCOMPONENT_H
#define LITBOARD_CYCLICCOMPONENT_H

#include "Component.h"


namespace lbd::comp {
    class CyclicComponent : public Component {
    public:
        CyclicComponent(std::function<void()> asyncTaskCycle, bool running, std::mutex& sleepMutex, std::condition_variable& sleepCondVar)
            : asyncTaskCycle(std::move(asyncTaskCycle)), taskThread(std::async(std::launch::async, [] {})),
              running(running), sleepMutex(sleepMutex), sleepCondVar(sleepCondVar) { }

        void keyboardConnected() override;
        void keyboardDisconnected() override;

    protected:
        void startAsyncCyclicTask();
        void stopAsyncCyclicTask();
        void launchAsyncCyclicTask();

        /**
         * Sleeps for the given duration while the following conditions are met: (connected && running).
         */
        void conditionalSleep(long long durationMs);

    private:
        const std::function<void()> asyncTaskCycle;
        std::future<void> taskThread;
        std::atomic<bool> connected = false;
        std::atomic<bool> running = false;

        std::mutex& sleepMutex;
        std::condition_variable& sleepCondVar;
    };
}


#endif //LITBOARD_CYCLICCOMPONENT_H
