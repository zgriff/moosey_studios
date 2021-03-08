//
//  InputControllerJS.h
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef InputControllerJS_h
#define InputControllerJS_h

#include <cugl/cugl.h>

class InputControllerJS {
private:
    /** Player id, to identify which keys map to this player */
    int _player;

    /** How much horizontal are we going? */
    float _horizontal;
    
    /** How much vertical are we going? */
    float _vertical;
    
    /** Whether the left joystick side is held */
    bool  _moveLeft;
    /** Whether the right joystick side is held */
    bool  _moveRight;
    /** Whether the up joystick side is held */
    bool  _moveUp;
    /** Whether the down joystick side is held */
    bool  _moveDown;

    
#pragma mark Internal Touch Management
    
    /** Information representing a single "touch" (possibly multi-finger) */
    struct TouchInstance {
        /** The anchor touch position (on start) */
        cugl::Vec2 position;
        /** The current touch time */
        cugl::Timestamp timestamp;
        /** The touch id(s) for future reference */
        std::unordered_set<Uint64> touchids;
    };
    
    /** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
    cugl::Rect _sbounds;
    /** The current touch location */
    TouchInstance _touch;
    
    /** Whether the virtual joystick is active */
    bool _joystick;
    /** The position of the virtual joystick */
    cugl::Vec2 _joycenter;
    
    /**
     * Populates the initial values of the TouchInstances
     */
    void clearTouchInstance(TouchInstance& touchInstance);
    
    /**
     * Returns the scene location of a touch
     *
     * Touch coordinates are inverted, with y origin in the top-left
     * corner. This method corrects for this and scales the screen
     * coordinates down on to the scene graph size.
     *
     * @return the scene location of a touch
     */
    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;
    
    /**
     * Processes movement for the floating joystick.
     *
     * This will register movement as left or right (or neither).  It
     * will also move the joystick anchor if the touch position moves
     * too far.
     *
     * @param  pos  the current joystick position
     */
    void processJoystick(const cugl::Vec2 pos);
    
public:
    
#pragma mark Constructors
    /**
     * Creates a new input controller with the default settings
     *
     * To use this controller, you will need to initialize it first
     */
    InputControllerJS();

    /**
     * Disposses this input controller, releasing all resources.
     */
    ~InputControllerJS() {}
    
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
    bool init(const cugl::Rect bounds);
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
#pragma mark Input Detection

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
    void  update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
#pragma mark Input Results
    /**
     * Returns the player for this controller
     *
     * @return the player for this controller
     */
    int getPlayer() const {
        return _player;
    }
    
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
    float getHorizontal() const {
        return _horizontal;
    }

    /**
     * Returns the amount of up/down movement.
     *
     * -1 = down, 1 = up 0 = still
     *
     * @return the amount of vertical movement.
     */
    float getVertical() const {
        return _vertical;
    }


    /**
     * Returns true if the virtual joystick is in use (touch only)
     *
     * @return true if the virtual joystick is in use (touch only)
     */
    bool withJoystick() const { return _joystick; }

    /**
     * Returns the scene graph position of the virtual joystick
     *
     * @return the scene graph position of the virtual joystick
     */
    cugl::Vec2 getJoystick() const { return _joycenter; }
    
#pragma mark Touch and Mouse Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     *
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
  
    /**
     * Callback for a mouse release event.
     *
     * @param event The associated event
     * @param previous The previous position of the touch
     * @param focus    Whether the listener currently has focus
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);
  
};

#endif /* RPSInputControllerJS_h */
