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
bool Settings::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE/dimen.width;
    } else {
        dimen *= SCENE_SIZE/dimen.height;
    }
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    _assets = assets;
    auto layer = assets->get<scene2::SceneNode>("settings");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    addChild(layer);
    
    _soundVolume = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("settings_soundvolume"));
//    _soundVolume->activate();
    _soundVolume->addListener([=](const std::string& name, float value) {
//        SoundController::setSoundVolume(value);
    });
    
    _backButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("settings_backbutton"));
    _backButton->activate();
    _backButton->addListener([=](const std::string& name, bool down) {
        _back = true;
    });
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void Settings::dispose() {
    _soundVolume = nullptr;
    _assets = nullptr;
    _active = false;
    _back = false;
}
                              
                              
    
