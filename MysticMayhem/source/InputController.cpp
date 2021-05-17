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
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::D
/** How far we must swipe up for a golf gesture */
#define SWIPE_LENGTH    50

/**
 * Creates a new input controller with the default settings
 *
 * To use this controller, you will need to initialize it first
 */
InputController::InputController() :
_keyDebug(false),
_debugPressed(false) {
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
    _moveStyle = Movement::TiltMove;
        
        // Only process keyboard on desktop
#ifndef CU_MOBILE
    success = Input::activate<Keyboard>();
#else
    success = Input::activate<Accelerometer>();
    Touchscreen* touch = Input::get<Touchscreen>();
    Uint32 touchListenerKey = touch->acquireKey();
    touch->addBeginListener(touchListenerKey,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(touchListenerKey,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
#endif
    return success;
}

void InputController::dispose() {
#ifndef CU_MOBILE
        Input::deactivate<Keyboard>();
#else
        Input::deactivate<Accelerometer>();
        Touchscreen* touch = Input::get<Touchscreen>();
        Uint32 touchListenerKey = touch->acquireKey();
        touch->removeBeginListener(touchListenerKey);
        touch->removeEndListener(touchListenerKey);
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
    Touchscreen* touch = Input::get<Touchscreen>();
    switch (_moveStyle) {
        case Movement::SwipeForce:
        case Movement::GolfMove:
            if(!processed){
                processed = true;
                mov.x = 1;
            }else{
                moveVec = Vec2::ZERO;
                mov.x = 0;
            }
            break;
        case Movement::TiltMove:
            _tiltVec = Input::get<Accelerometer>()->getAcceleration();
            break;
        default:
            _abilityPressed = false;
            if (_keydown && touch->touchCount() == 1) {
                CULog("touching");
                _abilityQueued = false;
                if (_dtouch.x < 640) {
                    mov.x = -1;
                }
                else {
                    mov.x = 1;
                }
            }
            else {
                mov.x = 0;
                if (touch->touchCount() > 1) {
                    cugl::Timestamp curr = Timestamp();
                    if (_abilityQueued && curr.ellapsedMillis(_abilityTimestamp) > 100) {
                        _abilityPressed = true;
                        _abilityQueued = false;
                    } else if (!_abilityQueued) {
                        _abilityQueued = true;
                        _abilityTimestamp.mark();
                    }
                    
                }
            }
            break;
    }
#else
//    CULog("input");
    // Figure out, based on which player we are, which keys
    // control our actions (depends on player).
    KeyCode up, left, right, down, space;
        up    = KeyCode::ARROW_UP;
        down  = KeyCode::ARROW_DOWN;
        left  = KeyCode::ARROW_LEFT;
        right = KeyCode::ARROW_RIGHT;
        space = KeyCode::SPACE;
    
    // Convert keyboard state into game commands
//    _didFire = false;
    mov = Vec2::ZERO;
    
    // Movement forward/backward
    Keyboard* keys = Input::get<Keyboard>();
    mov.y += keys->keyDown(up) ? 1 : 0;
    mov.y += keys->keyDown(down) ? -1 : 0;
    mov.x += keys->keyDown(left) ? -1 : 0;
    mov.x += keys->keyDown(right) ? 1 : 0;
    _abilityPressed = keys->keyDown(space);
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _debugPressed = _keyDebug;
#endif
}

#pragma mark -
#pragma mark Touch Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
    // All touches correspond to key up
    _keydown = true;
    _timestamp = event.timestamp;
    // Update the touch location for later gestures
    _dtouch = event.position;
}

/**
 * Callback for the end of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void InputController::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
    // Gesture has ended.  Give it meaning.
    switch (_moveStyle) {
        case Movement::SwipeForce:{
            Vec2 diff = event.position-_dtouch;
            Uint64 t = event.timestamp.ellapsedMillis(_timestamp);
            moveVec = diff.normalize() * (1000.0/t);
            processed = false;
            break;
        }
        case Movement::GolfMove:{
            moveVec.set(_dtouch - event.position);
            if (moveVec.length() < SWIPE_LENGTH ) {
                moveVec.set(Vec2(0.0f,0.0f));
            }
            processed = false;
            break;
        }
        default:
            _keydown = false;
            break;
    }
}

