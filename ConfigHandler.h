//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_CONFIGHANDLER_H
#define LITBOARD_CONFIGHANDLER_H

#include "Config.h"


namespace lbd {
    class ConfigHandler {
    public:
        void load();
        void save();
        [[nodiscard]] bool isLoaded() const;
        [[nodiscard]] const Config &getConfig() const;

    private:
        Config config;
        bool loaded = false;
    };
}


#endif //LITBOARD_CONFIGHANDLER_H
