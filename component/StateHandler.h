//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_STATEHANDLER_H
#define LITBOARD_STATEHANDLER_H

#include "Component.h"

namespace lbd::comp {
    enum KeyBoardEffects {

    };

    class StateHandler : public Component {
    public:
        [[nodiscard]] ComponentId getComponentId() const override;
    };
}


#endif //LITBOARD_STATEHANDLER_H
