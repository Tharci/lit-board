//
// Created by tmarc on 14/07/2021.
//

#include "LitBoardDriver.h"

lbd::LitBoardDriver &lbd::LitBoardDriver::getInstance() {
    static LitBoardDriver litBoardDriver;
    return litBoardDriver;
}

void lbd::LitBoardDriver::run() {
    configHandler.load();
    keyboardHandler.handleKeyboard();
}
