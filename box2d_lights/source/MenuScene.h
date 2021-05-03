//
//  MenuScene.h
//  Roshamboogie
//
//  Created by Zach Griffin on 3/25/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef MenuScene_h
#define MenuScene_h

#include <stdio.h>
#include <cugl/cugl.h>

#include "NetworkController.h"
#include "InputController.h"
#include "World.h"
#include "Globals.h"

/**
 * This class is a simple loading screen for asychronous asset loading.
 *
 * The screen will display a very minimal progress bar that displays the
 * status of the asset manager.  Make sure that all asychronous load requests
 * are issued BEFORE calling update for the first time, or else this screen
 * will think that asset loading is complete.
 *
 * Once asset loading is completed, it will display a play button.  Clicking
 * this button will inform the application root to switch to the gameplay mode.
 */
class MenuScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // VIEW
    /** The host game button */
    std::shared_ptr<cugl::scene2::Button> _hostButton;
    /** The join game button */
    std::shared_ptr<cugl::scene2::Button> _joinButton;
    /**join code text field*/
    std::shared_ptr<cugl::scene2::TextField> _codeField;
    std::shared_ptr<cugl::scene2::Slider> _slider;
    std::shared_ptr<cugl::scene2::Label> _label;
    float _sliderValue;
    /**true host false client*/
    bool _host;
    int _movement;
    bool _create;
    bool _join;
        

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new loading screen with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    MenuScene() : cugl::Scene2() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~MenuScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() override;

    /**
     * Initializes the scene contents, making it ready for loading
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);


#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the scene.
     *
     * This method will update the progress bar.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
//    virtual void update(float timestep) override;

    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
    bool isHost() {return _host;};
        
    int getMovement() {return _movement;};
    
    bool createPressed() { return _create; }
    bool joinPressed() { return _join; }

};

#endif /* MenuScene_h */
