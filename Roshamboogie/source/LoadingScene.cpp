//
//  LoadingScene.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "LoadingScene.h"
#include "NetworkController.h"

using namespace cugl;

/** This is the ideal size of the logo */
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
bool LoadingScene::init(const std::shared_ptr<AssetManager>& assets) {
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
    
    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    _assets->loadDirectory("json/loading.json");
    auto layer = assets->get<scene2::SceneNode>("load");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    
    _bar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("load_bar"));
    _brand = assets->get<scene2::SceneNode>("load_name");
    _button = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_play"));
    _button->addListener([=](const std::string& name, bool down) {
        _host = true;
        NetworkController::createGame();
        this->_active = down;
    });
    
    _button2 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_logo"));
    _button2->addListener([=](const std::string& name, bool down) {
        _host = false;
        _field->setVisible(true);
        _field->activate();
        _button->dispose();
        });

    _field = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("load_textfield_action"));
    _field->addTypeListener([=](const std::string& name, const std::string& value) {
        CULog("Change to %s", value.c_str());
        });
    _field->addExitListener([=](const std::string& name, const std::string& value) {
        CULog("Finish to %s", value.c_str());
        NetworkController::joinGame(value.c_str());
        this->_active = false;
        });
    Input::activate<TextInput>();
    _field->setVisible(false);

    Application::get()->setClearColor(Color4(192,192,192,255));
    addChild(layer);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LoadingScene::dispose() {
    // Deactivate the button (platform dependent)
    if (isPending()) {
        _button->deactivate();
    }
    _button = nullptr;
    _brand = nullptr;
    _bar = nullptr;
    _assets = nullptr;
    _progress = 0.0f;
}


#pragma mark -
#pragma mark Progress Monitoring
/**
 * The method called to update the game mode.
 *
 * This method updates the progress bar amount.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LoadingScene::update(float progress) {
    if (_progress < 1) {
        _progress = _assets->progress();
        if (_progress >= 1) {
            _progress = 1.0f;
            _bar->setVisible(false);
            _brand->setVisible(false);
            _button->setVisible(true);
            _button->activate();
            _button2->setVisible(true);
            _button2->activate();
        }
        _bar->setProgress(_progress);
    }
}

/**
 * Returns true if loading is complete, but the player has not pressed play
 *
 * @return true if loading is complete, but the player has not pressed play
 */
bool LoadingScene::isPending( ) const {
    return _button != nullptr && _button->isVisible();
}


