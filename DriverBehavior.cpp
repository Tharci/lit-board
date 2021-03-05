//
// Created by tharci on 2/13/21.
//

#include "DriverBehavior.h"

#include "LitBoardDriver.h"

void lbd::DriverBehavior::setActive(bool b) {
    this->active = b;
}

bool lbd::DriverBehavior::isActive() const {
    return active;
}
