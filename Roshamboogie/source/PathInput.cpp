//
//  PathInput.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "PathInput.h"

using namespace cugl;

#pragma mark Input Constants

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::D
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE

/** How far we must swipe up for a jump gesture */
#define SWIPE_LENGTH    50
/** The key for the event handlers */
#define LISTENER_KEY      1


#pragma mark -
#pragma mark Path Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
PathInput::PathInput() :
_active(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),
//_swingStart(false),
//_swingFinish(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_horizontal(0.0f),
_vertical(0.0f) {
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void PathInput::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        _active = false;
    }
}



bool PathInput::init(const Rect bounds) {
    _timestamp.mark();
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    
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
    touch->addMotionListener(LISTENER_KEY,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });
#endif
    _active = success;
    _dtouch = Vec2().ZERO;
    return success;
}


/**
 * Processes the currently cached inputs.
 *
 * This method is used to to poll the current input state.  This will poll the
 * keyboad and accelerometer.
 *
 * This method also gathers the delta difference in the touches. Depending on
 * the OS, we may see multiple updates of the same touch in a single animation
 * frame, so we need to accumulate all of the data together.
 */
void PathInput::update(float dt) {
#ifdef CU_MOBILE
    _keyDebug = false;
    _keyReset = false;
    _keyDebug = false;
#endif
//    _playerSwing.set(Vec2(0.0f,0.0f));
//    if (_swingVec != Vec2(0.0f,0.0f)) {
//        _playerSwing.set(_swingVec);
//        _swingVec.set(Vec2(0.0f,0.0f));
//    }
//    CULog("Swing vec is x: %f , y: %f", _swingVec.x,_swingVec.y);
}


Vec2 PathInput::touch2Screen(const Vec2 pos) const {
    float px = pos.x/_tbounds.size.width -_tbounds.origin.x;
    float py = pos.y/_tbounds.size.height-_tbounds.origin.y;
    Vec2 result;
    result.x = px*_sbounds.size.width +_sbounds.origin.x;
    result.y = (1-py)*_sbounds.size.height+_sbounds.origin.y;
    return result;
}


void PathInput::touchBeganCB(const TouchEvent &event, bool focus) {
    _dtouch = touch2Screen(event.position);
//    _timestamp.mark();
}

void PathInput::touchEndedCB(const TouchEvent &event, bool focus) {
//    _dtouch = Vec2().ZERO;
//    CULog("x: %f , y: %f",_swingVec.x,_swingVec.y);
}

void PathInput::touchesMovedCB(const TouchEvent &event, const Vec2 &previous, bool focus) {
    _dtouch.set(touch2Screen(event.position));
}


