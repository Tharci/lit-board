//
// Created by tharci on 2/13/21.
//

#ifndef LITBOARD_DRIVERBEHAVIOR_H
#define LITBOARD_DRIVERBEHAVIOR_H


#include "HIDDevice.h"


namespace lbd {
    class DriverBehavior {
    public:
        explicit DriverBehavior(HIDDevice* keyboard) : keyboard(keyboard) {}
        virtual ~DriverBehavior() = default;

        virtual void start() = 0;
        virtual void stopCallback() {}
        virtual void recieveMessage(const std::string& message) {}
        virtual void keyboardConnectedCallback() {}

        void setActive(bool b);
        [[nodiscard]] bool isActive() const;

    protected:
        HIDDevice* keyboard;

    private:
        bool active = true;
    };
}


#endif //LITBOARD_DRIVERBEHAVIOR_H
