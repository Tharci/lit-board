//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_MESSAGEHANDLER_H
#define LITBOARD_MESSAGEHANDLER_H

#include <cstdint>
#include "Component.h"


namespace lbd::comp {
    class MessageHandler : public Component {
    public:
        [[nodiscard]] ComponentId getComponentId() const override;
        void send(const Component& component, const uint8_t* data);

    private:
    };
}


#endif //LITBOARD_MESSAGEHANDLER_H
