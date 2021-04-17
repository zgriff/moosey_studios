//
//  InputController.hpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __INPUT_CONTROLLER_H__
#define __INPUT_CONTROLLER_H__

#include <cugl/cugl.h>
#include "MovementStyle.h"

class InputController {
private:
    cugl::Vec2 moveVec;
    cugl::Timestamp _timestamp;
    cugl::Timestamp _abilityTimestamp;
    bool _abilityQueued = false;
    bool processed;

    bool _abilityPressed = false;

    cugl::Vec2 mov;
    
    bool _keydown;
    cugl::Vec2 _dtouch;
//    /** Did we press the fire button? */
//    bool _didFire;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    
    Movement _moveStyle;

public:
    cugl::Vec3 _tiltVec;
        
    cugl::Vec3 getTiltVec(){
        return _tiltVec;
    }

    bool isAbilityPressed() { return _abilityPressed; };


    /**
     * Creates a new input controller with the default settings
     *
     * To use this controller, you will need to initialize it first
     */
    InputController();

    /**
     * Disposses this input controller, releasing all resources.
     */
    ~InputController() { dispose(); }
    
    cugl::Vec2 getMoveVec() const {
        return moveVec;
    }
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
    /**
     * Initializes a new input controller for the player.
     *
     * The game supports two players working against each other in hot seat mode.
     * We need a separate input controller for each player. In keyboard, this is
     * WASD vs. Arrow keys.  Doing this on mobile requires you to get a little
     * creative.
     *
     * @return true if the player was initialized correctly
     */
    bool init();

    /**
     * Reads the input for this player and converts the result into game logic.
     *
     * This is an example of polling input.  Instead of registering a listener,
     * we ask the controller about its current state.  When the game is running,
     * it is typically best to poll input instead of using listeners.  Listeners
     * are more appropriate for menus and buttons (like the loading screen).
     */
    void readInput();
    
    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
    bool didDebug() const { return _debugPressed; }
    
    cugl::Vec2 getMov(){
        return mov;
    }
    
    Movement getMoveStyle() const {return  _moveStyle;}
    
    void setMoveStyle(Movement m) {_moveStyle = m;}

#pragma mark -
#pragma mark Touch Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);

};

#endif /* InputController_hpp */
