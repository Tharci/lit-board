//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_APPINTEGRATION_H
#define LITBOARD_APPINTEGRATION_H

#include "Component.h"

#include <atomic>


namespace lbd::comp {
    enum class Apps {
        CSGO,
        RED_READ_2,
        TARKOV,
        WARCRAFT_III,
        DIABLO_II,
        DIABLO_III,
        WITCHER_III,
        STARCRAFT_I,
        STARCRAFT_II,
        OVERWATCH,
        BATTLEFIELD_1,
        BATTLEFIELD_4,
        BATTLEFIELD_5,
        GTA_5,
        GTA_VC,
        GTA_SA,
        NFS_UNDERGROUND,
        FORZA_HORIZON_3,
        FORZA_HORIZON_4,
        NOITA,
        VALORANT,
        VALHEIM,
        CYBERPUNK_2077,
    };

    class AppIntegration : public Component {
    public:
        AppIntegration();
        [[nodiscard]] ComponentId getComponentId() const override;

    private:
        void asyncTaskCycle();
    };
}


#endif //LITBOARD_APPINTEGRATION_H
