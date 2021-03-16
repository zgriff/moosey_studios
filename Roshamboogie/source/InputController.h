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

class InputController {
private:
    /** Player id, to identify which keys map to this player */
    int _player;

    cugl::Vec2 mov;
    
    bool _keydown;
    cugl::Vec2 _dtouch;
//    /** Did we press the fire button? */
//    bool _didFire;

public:
    /**
     * Returns the player for this controller
     *
     * @return the player for this controller
     */
    int getPlayer() const {
        return _player;
    }

//    /**
//     * Returns whether the fire button was pressed.
//     *
//     * @return whether the fire button was pressed.
//     */
//    bool didPressFire() const {
//        return _didFire;
//    }

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
    
    cugl::Vec2 getMov(){
        return mov;
    }

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
