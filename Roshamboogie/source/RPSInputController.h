//
//  RPSInputController.hpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __RPS_INPUT_CONTROLLER_H__
#define __RPS_INPUT_CONTROLLER_H__

#include <stdio.h>

class InputController {
private:
    /** Player id, to identify which keys map to this player */
    int _player;

    /** How much forward are we going? */
    float _forward;
    
    /** How much are we turning? */
    float _turning;
    
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
    
    /**
     * Returns the amount of forward movement.
     *
     * -1 = backward, 1 = forward, 0 = still
     *
     * @return amount of forward movement.
     */
    float getForward() const {
        return _forward;
    }

    /**
     * Returns the amount to turn the ship.
     *
     * -1 = clockwise, 1 = counter-clockwise, 0 = still
     *
     * @return amount to turn the ship.
     */
    float getTurn() const {
        return _turning;
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
    ~InputController() {}
    
    /**
     * Initializes a new input controller for the specified player.
     *
     * The game supports two players working against each other in hot seat mode.
     * We need a separate input controller for each player. In keyboard, this is
     * WASD vs. Arrow keys.  Doing this on mobile requires you to get a little
     * creative.
     *
     * @param id Player id number (0..1)
     *
     * @return true if the player was initialized correctly
     */
    bool init(int id);

    /**
     * Reads the input for this player and converts the result into game logic.
     *
     * This is an example of polling input.  Instead of registering a listener,
     * we ask the controller about its current state.  When the game is running,
     * it is typically best to poll input instead of using listeners.  Listeners
     * are more appropriate for menus and buttons (like the loading screen).
     */
    void readInput();
};

#endif /* RPSInputController_hpp */
