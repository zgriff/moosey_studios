//
//  InputControllerJS.cpp
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <cugl/cugl.h>
#include "InputControllerJS.h"

using namespace cugl;

#pragma mark Input Constants

/** The key for the event handlers */
#define LISTENER_KEY      1

/** This defines the joystick "deadzone" (how far we must move) */
#define JSTICK_DEADZONE  15
/** This defines the joystick radial size (for reseting the anchor) */
#define JSTICK_RADIUS    25
/** How far to display the virtual joystick above the finger */
#define JSTICK_OFFSET    0
/** How far we must swipe up for a jump gesture */
#define SWIPE_LENGTH    50
/** How fast a double click must be in milliseconds */
#define DOUBLE_CLICK    400

#pragma mark -
#pragma mark Input Controller

/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
InputControllerJS::InputControllerJS() :
_horizontal(0.0f),
_vertical(0.0f),
_joystick(false){
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputControllerJS::dispose() {
#ifndef CU_MOBILE
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->removeBeginListener(LISTENER_KEY);
    touch->removeEndListener(LISTENER_KEY);
#endif
}

/**
 * Initializes the input control for the given bounds
 *
 * The bounds are the bounds of the scene graph.  This is necessary because
 * the bounds of the scene graph do not match the bounds of the display.
 * This allows the input device to do the proper conversion for us.
 *
 * @param bounds    the scene graph bounds
 *
 * @return true if the controller was initialized successfully
 */
bool InputControllerJS::init(const Rect bounds) {
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    
    clearTouchInstance(_touch);
    
#ifndef CU_MOBILE

#else
    CULog("mobile %d", 20);
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
void InputControllerJS::update(float dt) {
    // Directional controls
    _horizontal = 0.0f;
    if (_moveRight) {
        _horizontal += 1.0f;
    }
    if (_moveLeft) {
        _horizontal -= 1.0f;
    }
    _vertical = 0.0f;
    if (_moveUp) {
        _vertical += 1.0f;
    }
    if (_moveDown) {
        _vertical -= 1.0f;
    }
}

#pragma mark -
#pragma mark Touch Controls

/**
 * Populates the initial values of the input TouchInstance
 */
void InputControllerJS::clearTouchInstance(TouchInstance& touchInstance) {
    touchInstance.touchids.clear();
    touchInstance.position = Vec2::ZERO;
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
Vec2 InputControllerJS::touch2Screen(const Vec2 pos) const {
    float px = pos.x/_tbounds.size.width -_tbounds.origin.x;
    float py = pos.y/_tbounds.size.height-_tbounds.origin.y;
    Vec2 result;
    result.x = px*_sbounds.size.width +_sbounds.origin.x;
    result.y = (1-py)*_sbounds.size.height+_sbounds.origin.y;
    return result;
}


/**
 * Processes movement for the floating joystick.
 *
 * This will register movement as left or right (or neither).  It
 * will also move the joystick anchor if the touch position moves
 * too far.
 *
 * @param  pos  the current joystick position
 */
void InputControllerJS::processJoystick(const cugl::Vec2 pos) {
    Vec2 diff =  _touch.position-pos;

    // Reset the anchor if we drifted too far
    if (diff.lengthSquared() > JSTICK_RADIUS*JSTICK_RADIUS) {
        diff.normalize();
        diff *= (JSTICK_RADIUS+JSTICK_DEADZONE)/2;
        _touch.position = pos+diff;
    }
//    _touch.position.y = pos.y;
    _joycenter = touch2Screen(_touch.position);
    _joycenter.y = 100;
    _joycenter.x = 50;
    
    if (std::fabsf(diff.x) > JSTICK_DEADZONE) {
        _joystick = true;
        if (diff.x > 0) {
            _moveLeft = true;
            _moveRight = false;
            _moveUp = false;
            _moveDown = false;
        } else {
            _moveLeft = false;
            _moveRight = true;
            _moveUp = false;
            _moveDown = false;
        }
    }
    else if (std::fabsf(diff.y) > JSTICK_DEADZONE) {
        if (diff.y > 0) {
            _moveUp = true;
            _moveDown = false;
            _moveLeft = false;
            _moveRight = false;
        } else {
            _moveUp = false;
            _moveDown = true;
            _moveLeft = false;
            _moveRight = false;
        }
    } else {
        _joystick = false;
        _moveLeft = false;
        _moveRight = false;
        _moveUp = false;
        _moveDown = false;
    }
}

#pragma mark -
#pragma mark Touch and Mouse Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputControllerJS::touchBeganCB(const TouchEvent& event, bool focus) {
//    CULog("Touch began %lld", event.touch);
//    Vec2 pos = event.position;
    // Only process if no touch in zone
    if (_touch.touchids.empty()) {
        _touch.position = event.position;
        _touch.timestamp.mark();
        _touch.touchids.insert(event.touch);

        _joystick = true;
        _joycenter = touch2Screen(event.position);
        _joycenter.y += JSTICK_OFFSET;
        _joycenter.x += JSTICK_OFFSET;
    }
}

 
/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputControllerJS::touchEndedCB(const TouchEvent& event, bool focus) {
    // Reset all keys that might have been set
//    Vec2 pos = event.position;
//    CULog("Touch ended %lld", event.touch);
    if (_touch.touchids.find(event.touch) != _touch.touchids.end()) {
        _touch.touchids.clear();
        _moveLeft = false;
        _moveRight = false;
        _moveUp = false;
        _moveDown = false;
        _joystick = false;
    }
}


/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void InputControllerJS::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    Vec2 pos = event.position;
    if (_touch.touchids.find(event.touch) != _touch.touchids.end()) {
        processJoystick(pos);
    }
}
