//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_APPINTEGRATION_H
#define LITBOARD_APPINTEGRATION_H

#include "Component.h"


namespace lbd::comp {
    class AppIntegration : public Component {
    public:
        [[nodiscard]] ComponentId getComponentId() const override;
    };
}


#endif //LITBOARD_APPINTEGRATION_H
