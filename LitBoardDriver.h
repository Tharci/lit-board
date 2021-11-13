//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_LITBOARDDRIVER_H
#define LITBOARD_LITBOARDDRIVER_H

#include "ConfigHandler.h"
#include "KeyboardHandler.h"
#include "Component.h"
#include "MessageHandler.h"
#include "LiveWeather.h"
#include "AppIntegration.h"
#include "AudioVisualizer.h"
#include "NotificationHandler.h"
#include "ServerHandler.h"
#include "StateHandler.h"

#include <unordered_map>


namespace lbd {
    /*
     * Singleton
     */
    class LitBoardDriver {
    public:
        static LitBoardDriver& getInstance();
        LitBoardDriver(const LitBoardDriver&) = delete;
        LitBoardDriver& operator= (const LitBoardDriver&) = delete;

        void run();
        ConfigHandler& getConfigHandler();
        KeyboardHandler& getKeyboardHandler();
        comp::MessageHandler& getMessageHandler();
        std::unordered_map<comp::ComponentId, comp::Component*>& getComponents();
        void addComponent(comp::Component* component);

    private:
        LitBoardDriver();
        ~LitBoardDriver() = default;

        KeyboardHandler keyboardHandler;
        ConfigHandler configHandler;

        /*
         * Components
         */
        std::unordered_map<comp::ComponentId, comp::Component*> components;
        comp::MessageHandler messageHandler;
        comp::LiveWeather liveWeather;
        comp::AppIntegration appIntegration;
        comp::AudioVisualizer audioVisualizer;
        comp::NotificationHandler notificationHandler;
        comp::ServerHandler serverHandler;
        comp::StateHandler stateHandler;
    };
}


#endif //LITBOARD_LITBOARDDRIVER_H
