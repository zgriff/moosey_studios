//
//  InputController.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "InputController.h"

#include <cugl/cugl.h>

using namespace cugl;
/** The key for the event handlers */
#define LISTENER_KEY        1

/**
 * Creates a new input controller with the default settings
 *
 * To use this controller, you will need to initialize it first
 */
InputController::InputController() {
}

/**
 * Initializes a new input controller for the  player.
 *
 * The game supports two players working against each other in hot seat mode.
 * We need a separate input controller for each player. In keyboard, this is
 * WASD vs. Arrow keys.  Doing this on mobile requires you to get a little
 * creative.
 *
 * @return true if the player was initialized correctly
 */
bool InputController::init() {
    bool success = true;
        
        // Only process keyboard on desktop
#ifndef CU_MOBILE
    success = Input::activate<Keyboard>();
#else
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
#endif
    return success;
}

void InputController::touchBeganCB(const TouchEvent& event, bool focus){
    _timestamp = event.timestamp;
    _dtouch = event.position;
}

void InputController::touchEndedCB(const TouchEvent& event, bool focus){
    Vec2 diff = event.position-_dtouch;
    Uint64 t = event.timestamp.ellapsedMillis(_timestamp);
//    diff.getAngle();
//    _accel = diff.getAngle();
//    _accel_amount = 1/t
    moveVec = diff.normalize() * (1000.0/t);
    processed = false;
}

void InputController::dispose() {
#ifndef CU_MOBILE
        Input::deactivate<Keyboard>();
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
}

/**
 * Reads the input for this player and converts the result into game logic.
 *
 * This is an example of polling input.  Instead of registering a listener,
 * we ask the controller about its current state.  When the game is running,
 * it is typically best to poll input instead of using listeners.  Listeners
 * are more appropriate for menus and buttons (like the loading screen).
 */
void InputController::readInput() {
#ifdef CU_MOBILE
    // YOU NEED TO PUT SOME CODE HERE
    if(!processed){
        processed = true;
    }else{
        moveVec = Vec2::ZERO;
    }
#else
   

#endif
}
