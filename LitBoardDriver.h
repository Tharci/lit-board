//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_LITBOARDDRIVER_H
#define LITBOARD_LITBOARDDRIVER_H

#include "ConfigHandler.h"
#include "KeyboardHandler.h"
#include "component/Component.h"
#include "component/MessageHandler.h"
#include <hash_map>


namespace lbd {
    class LitBoardDriver {
    public:
        static LitBoardDriver& getInstance();

        LitBoardDriver() = default;
        LitBoardDriver(const LitBoardDriver&) = delete;
        LitBoardDriver& operator= (const LitBoardDriver&) = delete;
        ~LitBoardDriver();


        void run();
        ConfigHandler& getConfigHandler();
        KeyboardHandler& getKeyboardHandler();
        comp::MessageHandler& getMessageHandler();
        std::hash_map<ComponentId, comp::Component*>& getComponents();

    private:
        KeyboardHandler keyboardHandler;
        ConfigHandler configHandler;
        comp::MessageHandler messageHandler;
        std::hash_map<ComponentId, comp::Component> components;
    };
}


#endif //LITBOARD_LITBOARDDRIVER_H
