//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_NOTIFICATIONHANDLER_H
#define LITBOARD_NOTIFICATIONHANDLER_H

#include "Component.h"


namespace lbd::comp {
    class NotificationHandler : public Component {
    public:
        [[nodiscard]] ComponentId getComponentId() const override;
    };
}


#endif //LITBOARD_NOTIFICATIONHANDLER_H
