//
//  EndScene.cpp
//  Roshamboogie
//
//  Created by Ikra Monjur on 4/3/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "EndScene.h"

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
bool EndScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_SIZE/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    _assets = assets;
    auto layer = assets->get<scene2::SceneNode>("end");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    addChild(layer);
    
    _playAgainButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("end_playagain"));
    _playAgainButton->activate();
//    _playAgainButton->addListener([=](const std::string& name, bool down) {
//        this->_active = down;
//    });

    

    Application::get()->setClearColor(Color4(192,192,192,255));
    return true;
}

/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void EndScene::dispose() {
    if (isPending()) {
        _playAgainButton->deactivate();
    }
    _assets = nullptr;
    _playAgainButton = nullptr;
    
}

/*
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void EndScene::update(float timestep) {
}

/* @return true if loading is complete, but the player has not pressed play
*/
bool EndScene::isPending() const {
   return _playAgainButton != nullptr && _playAgainButton->isVisible();
}
