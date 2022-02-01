//
// Created by tmarc on 14/07/2021.
//

#include <fstream>

#include "ConfigHandler.h"
#include "json.hpp"

using nlohmann::json;


void lbd::ConfigHandler::load() {
    config.city = "Szentendre";

    try {
        std::ifstream file("config.json");
        json js;
        file >> js;

        config.city = js["components"]["LiveWeather"]["city"];

        file.close();
        loaded = true;
    } catch (...) {}
}

const lbd::Config& lbd::ConfigHandler::getConfig() const {
    if (!isLoaded())
        throw std::exception("Config is not loaded.");
    return config;
}

bool lbd::ConfigHandler::isLoaded() const {
    return loaded;
}
