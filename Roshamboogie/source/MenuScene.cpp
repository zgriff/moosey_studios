//
//  MatchmakingScene.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/25/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "MenuScene.h"
#include <time.h>

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
bool MenuScene::init(const std::shared_ptr<AssetManager>& assets) {
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
    auto layer = assets->get<scene2::SceneNode>("menu");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    addChild(layer);
    
    _hostButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_host"));
    _hostButton->addListener([=](const std::string& name, bool down) {
        _host = true;
        NetworkController::createGame();
        _joinButton->deactivate();
        _slider->setVisible(false);
        _slider->deactivate();
        _label->setVisible(false);
        this->_active = down;
    });
    
    _joinButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_join"));
    _joinButton->addListener([=](const std::string& name, bool down) {
        _host = false;
        _slider->setVisible(false);
        _slider->deactivate();
        _label->setVisible(false);
        _hostButton->setVisible(false);
        _hostButton->deactivate();
        _joinButton->setVisible(false);
        _codeField->activate();
        _codeField->setVisible(true);
    });
    
    _slider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("menu_slider"));
    _label  = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_slider_label"));
    _label->setText("Always Forward");
    _movement = 0;
    _slider->addListener([=](const std::string& name, float value) {
        if (value != _sliderValue) {
            _sliderValue = value;
            _movement = static_cast<int>(value);
            switch (_movement) {
                case 0:
                    _label->setText("Always Forward");
                    break;
                case 1:
                    _label->setText("Swipe Force");
                    break;
                default:
                    _label->setText("Tilt to Move");
                    break;
            }
        }
    });
    

    _codeField = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_joincode"));
    _codeField->addTypeListener([=](const std::string& name, const std::string& value) {
        CULog("Change to %s", value.c_str());
        });
    _codeField->addExitListener([=](const std::string& name, const std::string& value) {
        CULog("Finish to %s", value.c_str());
        NetworkController::joinGame(value);
        clock_t oldTime = clock();
        //this is here as a hacky way to get network->receive, but can be removed when the network library is updated
        while (clock() - oldTime < 2 * CLOCKS_PER_SEC) {
            NetworkController::step();
        }
        if (NetworkController::getNumPlayers() > 1) {
        this->_active = false;
        }
        });
    
    _usernameField = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_username_action"));
    _usernameField->addTypeListener([=](const std::string& name, const std::string& value) {
        CULog("Change to %s", value.c_str());
        });
    _usernameField->addExitListener([=](const std::string& name, const std::string& value) {
        CULog("Finish to %s", value.c_str());
        //TODO: set username in network controller
        });
    _usernameField->setVisible(true);
    
    Input::activate<TextInput>();
    _codeField->setVisible(false);
    if(_active) {
        _slider->activate();
    }

    Application::get()->setClearColor(Color4(192,192,192,255));
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    _hostButton = nullptr;
    _joinButton = nullptr;
    Input::deactivate<TextInput>();
    _codeField = nullptr;
    _slider = nullptr;
    _assets = nullptr;
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
//void MenuScene::update(float progress) {
//
//    _hostButton->setVisible(true);
//    _hostButton->activate();
//    _joinButton->setVisible(true);
//    _joinButton->activate();
//
//}



/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void MenuScene::setActive(bool value) {
    if (value && (!_hostButton->isActive() || !_joinButton->isActive())) {
        _hostButton->activate();
        _joinButton->activate();
    } else if (!value && (_hostButton->isActive() || _joinButton->isActive())) {
        _hostButton->deactivate();
        _joinButton->deactivate();
        _codeField->deactivate();
    }
}
