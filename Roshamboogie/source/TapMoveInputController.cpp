//
//  TapMoveInputController.cpp
//  Roshamboogie
//
//  Created by Thomas Chen on 3/11/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "TapMoveInputController.h"

#include <cugl/cugl.h>

using namespace cugl;
/** The key for the event handlers */
#define LISTENER_KEY        1

/**
 * Creates a new input controller with the default settings
 *
 * To use this controller, you will need to initialize it first
 */
TapMoveInputController::TapMoveInputController() {
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
bool TapMoveInputController::init(const Rect bounds) {
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();

    // Only process keyboard on desktop
#ifndef CU_MOBILE
    success = Input::activate<Keyboard>();
#else
    success = Input::activate<Accelerometer>();
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

void TapMoveInputController::dispose() {
#ifndef CU_MOBILE
    Input::deactivate<Keyboard>();
#else
    Input::deactivate<Accelerometer>();
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
void TapMoveInputController::readInput() {
#ifdef CU_MOBILE
    Input::deactivate<Keyboard>();
#else
    // Figure out, based on which player we are, which keys
    // control our actions (depends on player).
    KeyCode up, left, right, down;
    up = KeyCode::ARROW_UP;
    down = KeyCode::ARROW_DOWN;
    left = KeyCode::ARROW_LEFT;
    right = KeyCode::ARROW_RIGHT;

    // Convert keyboard state into game commands
    mov = Vec2::ZERO;

    // Movement forward/backward
    Keyboard* keys = Input::get<Keyboard>();
    mov.y += keys->keyDown(up) ? 1 : 0;
    mov.y += keys->keyDown(down) ? -1 : 0;
    mov.x += keys->keyDown(left) ? -1 : 0;
    mov.x += keys->keyDown(right) ? 1 : 0;

    Mouse* mouse = Input::get<Mouse>();
    if (mouse->buttonDown().hasLeft()) {
        _pressed = true;
        _mousePosition = touch2Screen(mouse->pointerPosition());
    }
    else {
        _pressed = false;
    }

#endif
}

void TapMoveInputController::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
    // Update the touch location for later gestures
    _mousePosition.set(touch2Screen(event.position));
    _pressed = true;
}

void TapMoveInputController::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
    _pressed = false;
}

/**
 * Returns the scene location of a touch
 *
 * Touch coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a touch
 */
cugl::Vec2 TapMoveInputController::touch2Screen(const cugl::Vec2 pos) const {
    float px = pos.x / _tbounds.size.width - _tbounds.origin.x;
    float py = pos.y / _tbounds.size.height - _tbounds.origin.y;
    cugl::Vec2 result;
    result.x = px * _sbounds.size.width + _sbounds.origin.x;
    result.y = (1 - py) * _sbounds.size.height + _sbounds.origin.y;
    /*CULog(_tbounds.toString().c_str());
    CULog(_sbounds.toString().c_str());*/
    /*CULog("input is");
    CULog(pos.toString().c_str());
    CULog("result is");
    CULog(result.toString().c_str());*/
    return result;
}