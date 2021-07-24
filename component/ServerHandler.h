//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_SERVERHANDLER_H
#define LITBOARD_SERVERHANDLER_H

#include "Component.h"


namespace lbd::comp {
    class ServerHandler : public Component {
    public:
        [[nodiscard]] ComponentId getComponentId() const override;
    };
}


#endif //LITBOARD_SERVERHANDLER_H
