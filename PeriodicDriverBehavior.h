//
// Created by tharci on 2/15/21.
//

#ifndef LITBOARD_PERIODICDRIVERBEHAVIOR_H
#define LITBOARD_PERIODICDRIVERBEHAVIOR_H

#include "DriverBehavior.h"

namespace lbd {
    class PeriodicDriverBehavior: public DriverBehavior {
    public:
        explicit PeriodicDriverBehavior(HIDDevice* keyboard, unsigned cycleTimeMs)
            : DriverBehavior(keyboard), cycleTimeMs(cycleTimeMs) {}

        virtual void start() override {}
        virtual void tick() = 0;

        [[nodiscard]] unsigned getCycleTimeMs() const {
            return cycleTimeMs;
        }
    private:
        unsigned cycleTimeMs;
    };
}


#endif //LITBOARD_PERIODICDRIVERBEHAVIOR_H
