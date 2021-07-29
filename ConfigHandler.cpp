//
// Created by tmarc on 14/07/2021.
//

#include "ConfigHandler.h"

void lbd::ConfigHandler::load() {
    config.city = "Szentendre";
    loaded = true;
}

const lbd::Config& lbd::ConfigHandler::getConfig() const {
    if (!isLoaded())
        throw std::exception("Config is not loaded.");
    return config;
}

void lbd::ConfigHandler::save() {

}

bool lbd::ConfigHandler::isLoaded() const {
    return loaded;
}
