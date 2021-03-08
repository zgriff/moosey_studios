//
//  SwipeMoveInput.cpp
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//
#include <cugl/cugl.h>
#include "SwipeMoveInput.h"

using namespace cugl;

#define RANGE_CLAMP(x,y,z)  (x < y ? y : (x > z ? z : x))
/** Historical choice from Marmalade */
#define INPUT_MAXIMUM_FORCE         1000.0f
/** Adjustment factor for touch input */
#define X_ADJUST_FACTOR             100.0f
#define Y_ADJUST_FACTOR             100.0f
/** The key for the event handlers */
#define LISTENER_KEY                1

#pragma mark -
#pragma mark Ship Input
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
SwipeMoveInput::SwipeMoveInput() :
_forceLeft(0.0f),
_forceRight(0.0f),
_forceUp(0.0f),
_forceDown(0.0f)
{
}


/* Deactivates this input controller, releasing all listeners.
*
* This method will not dispose of the input controller. It can be reused
* once it is reinitialized.
*/
void SwipeMoveInput::dispose() {
#ifndef CU_TOUCH_SCREEN

#else
       Touchscreen* touch = Input::get<Touchscreen>();
       touch->removeBeginListener(LISTENER_KEY);
       touch->removeEndListener(LISTENER_KEY);
#endif
   }

/**
 * Initializes the input control for the given drawing scale.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory.  However, it still does not activate
 * the listeners.  You must call start() do that.
 *
 * @return true if the controller was initialized successfully
 */
bool SwipeMoveInput::init() {
    _timestamp.mark();
    bool success = true;
    
// Only process keyboard on desktop
#ifndef CU_TOUCH_SCREEN

#else
    Touchscreen* touch = Input::get<Touchscreen>();

    touch->addBeginListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
    touch->addMotionListener(LISTENER_KEY,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });
#endif
    
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
void SwipeMoveInput::update(float dt) {


#ifndef CU_TOUCH_SCREEN
#else
    // MOBILE CONTROLS
    // Otherwise, uses touch
#endif

    
#ifdef CU_TOUCH_SCREEN
#endif
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void SwipeMoveInput::clear() {
    
    _forceLeft  = 0.0f;
    _forceRight = 0.0f;
    _forceUp    = 0.0f;
    _forceDown  = 0.0f;
    
    _dtouch = cugl::Vec2::ZERO;
    _timestamp.mark();
}

#pragma mark -
#pragma mark Touch Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
 void SwipeMoveInput::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
     // Update the touch location for later gestures
     _dtouch.set(event.position);
}


 
 /**
  * Callback for the end of a touch event
  *
  * @param t     The touch information
  * @param event The associated event
  */
 void SwipeMoveInput::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
     _inputThrust = cugl::Vec2::ZERO;
 }

/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void SwipeMoveInput::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    // Check for a double tap.
    _timestamp = event.timestamp;
    
    
    // Move the ship in this direction
    Vec2 finishTouch = event.position-_dtouch;
    finishTouch.x = RANGE_CLAMP(finishTouch.x, -INPUT_MAXIMUM_FORCE, INPUT_MAXIMUM_FORCE);
    finishTouch.y = RANGE_CLAMP(finishTouch.y, -INPUT_MAXIMUM_FORCE, INPUT_MAXIMUM_FORCE);
    
    // Go ahead and apply to thrust now.
    _inputThrust.x = finishTouch.x/X_ADJUST_FACTOR;
    _inputThrust.y = finishTouch.y/-Y_ADJUST_FACTOR;  // Touch coords
}



