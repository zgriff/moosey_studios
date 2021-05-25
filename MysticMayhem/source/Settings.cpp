//
//  Settings.cpp
//  MysticMayhem
//
//  Created by Ikra Monjur on 5/2/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <stdio.h>
#include "Settings.h"
#include "SoundController.h"

using namespace cugl;

#define SCENE_SIZE  1024

#pragma mark -
#pragma mark Constructors


Settings::Settings(const std::shared_ptr<cugl::AssetManager>& assets, bool inGame){
    init(assets, inGame);
}

/**
 * Initializes the controller contents, making it ready for loading
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool Settings::init(const std::shared_ptr<AssetManager>& assets, bool inGame) {
    scene2::SceneNode::init();
    setAnchor(0.5,0.5);
    setVisible(true);
    
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_SIZE/dimen.width;
    setContentSize(Size(dimen.height,dimen.height));
    setPosition(dimen.width/2, dimen.height/2);
    
    if (assets == nullptr) {
        return false;
    }
    
    _leaveGame = false;
    
//    _assets = assets;
    auto layer = assets->get<scene2::SceneNode>("settings");
    layer->setContentSize(Size(dimen.height,dimen.height));
//    layer->doLayout(); // This rearranges the children to fit the screen
    addChild(layer);
    doLayout();
    
    _soundVolume = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("settings_soundvolume"));
    _soundVolume->activate();
    _soundVolume->addListener([=](const std::string& name, float value) {
        SoundController::setSoundVolume(value);
    });
    
    _backButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settings_backbutton"));
    _backButton->activate();
    _backButton->addListener([=](const std::string& name, bool down) {
        _back = down;
    });
    
    _leavegameButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settings_leavegamebutton"));
    
    _inGame = inGame;
    // initialize leave game button if in game
    if (inGame) {
        _leavegameButton->setVisible(true);
        _leavegameButton->activate();
        _leavegameButton->addListener([=](const std::string& name, bool down) {
            _leaveGame = down;
        });
    }
    else {
        _leavegameButton->setVisible(false);
        _leavegameButton->deactivate();
    }
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void Settings::dispose() {
//    removeAllChildren();
    _musicVolume = nullptr;
    _leavegameButton = nullptr;
    _soundVolume = nullptr;
    _backButton = nullptr;
    _back = false;
    _leaveGame = false;
}

void Settings::clearListeners() {
    if (_musicVolume != nullptr) {
        _musicVolume->clearListeners();
    }
    if (_leavegameButton != nullptr) {
        _leavegameButton->clearListeners();
    }
    if (_soundVolume != nullptr) {
        _soundVolume->clearListeners();
    }
    if (_backButton != nullptr) {
        _backButton->clearListeners();
    }

}

void Settings::setActive(bool b) {
    if (b) {
        _soundVolume->activate();
        _backButton->activate();
        if (_inGame) {
            _leavegameButton->activate();
        }
    }
    else {
        _soundVolume->deactivate();
        _backButton->deactivate();
        _back = false;
        if (_inGame) {
            _leaveGame = false;
            _leavegameButton->deactivate();
        }
    }
}
                              
                              
    
