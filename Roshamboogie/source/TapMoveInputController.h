//
//  TapMoveInputController.hpp
//  Roshamboogie
//
//  Created by Thomas Chen on 3/11/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __TAP_MOVE_INPUT_CONTROLLER_H__
#define __TAP_MOVE_INPUT_CONTROLLER_H__

#include <cugl/cugl.h>

class TapMoveInputController {
private:
    /** Player id, to identify which keys map to this player */
    int _player;

    bool _pressed;
    cugl::Vec2 _mousePosition;

    cugl::Vec2 mov;

    /** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
    cugl::Rect _sbounds;

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
    * Creates a new input controller with the default settings
    *
    * To use this controller, you will need to initialize it first
    */
    TapMoveInputController();

    /**
     * Disposses this input controller, releasing all resources.
     */
    ~TapMoveInputController() { dispose(); }

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
    bool init(const cugl::Rect bounds);

    /**
     * Reads the input for this player and converts the result into game logic.
     *
     * This is an example of polling input.  Instead of registering a listener,
     * we ask the controller about its current state.  When the game is running,
     * it is typically best to poll input instead of using listeners.  Listeners
     * are more appropriate for menus and buttons (like the loading screen).
     */
    void readInput();

    float getPressed() const {
        return _pressed;
    }

    cugl::Vec2 getMousePosition() const {
        return _mousePosition;
    }

    void setMousePosition(cugl::Vec2 pos) {
        _mousePosition = pos;
    }

    cugl::Vec2 getMov() {
        return mov;
    }

    void touchBeganCB(const cugl::TouchEvent& event, bool focus);
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);

    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;
};

#endif /* TapMoveInputController_hpp */
