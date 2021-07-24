//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_LIVEWEATHER_H
#define LITBOARD_LIVEWEATHER_H

#include "Component.h"


namespace lbd::comp {
    class LiveWeather : public Component {
    public:
        [[nodiscard]] ComponentId getComponentId() const override;
    };
}


#endif //LITBOARD_LIVEWEATHER_H
