//
//  LobbyScene.h
//  Roshamboogie
//
//  Created by Zach Griffin on 3/25/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef LobbyScene_h
#define LobbyScene_h

#include <stdio.h>
#include <cugl/cugl.h>

#include "NetworkController.h"
#include "InputController.h"
#include "World.h"
#include "Globals.h"
#include "MapConstants.h"
#include "Settings.h"

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
class LobbyScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // VIEW
    std::shared_ptr<Player> _playerCustom;
    std::shared_ptr<cugl::scene2::Button> _hatForwardButton;
    std::shared_ptr<cugl::scene2::Button> _hatBackButton;
    std::shared_ptr<cugl::scene2::Button> _skinForwardButton;
    std::shared_ptr<cugl::scene2::Button> _skinBackButton;
    std::shared_ptr<cugl::scene2::Button> _eleForwardButton;
    std::shared_ptr<cugl::scene2::Button> _eleBackButton;
    
    std::shared_ptr<cugl::scene2::Button> _mapNextButton;
    std::shared_ptr<cugl::scene2::Button> _mapPrevButton;
    
    std::shared_ptr<cugl::scene2::Button> _startButton;
    std::shared_ptr<cugl::scene2::Button> _map1Button;
    std::shared_ptr<cugl::scene2::Button> _map2Button;
    std::shared_ptr<cugl::scene2::Button> _map3Button;
    std::shared_ptr<cugl::scene2::Button> _map4Button;
    string _selectedMap = GRASS_MAP_KEY;
    std::shared_ptr<cugl::scene2::Label> _roomId;
    std::vector<std::shared_ptr<cugl::scene2::Label>> _playerLabels;
    std::shared_ptr<Settings> _settingsNode;
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    std::shared_ptr<cugl::scene2::Button> _settingsButton;
    
    
    
    /**true host false client*/
    std::string _currRoomId;
    bool _host;
    bool _playAgain;
//    int _movement;
        

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new loading screen with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    LobbyScene() : cugl::Scene2() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LobbyScene() { dispose(); }

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
    virtual void update(float timestep) override;

    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
    void displayLobby();
    
    bool isHost() {return _host;};
    
    bool isPlayAgain() { return _playAgain; }
    
    void setPlayAgain(bool p) { _playAgain = p;}

    string getSelectedMap() { return _selectedMap; };
    void setSelectedMap(string map) { _selectedMap = map; };
    
    std::shared_ptr<Settings> getSettings() { return _settingsNode; }
        
};

#endif /* LobbyScene_h */
