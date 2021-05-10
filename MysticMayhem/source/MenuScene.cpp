//
//  MatchmakingScene.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/25/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "MenuScene.h"
#include <time.h>
#include "Settings.h"

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
    
    _assets = assets;
    auto layer = assets->get<scene2::SceneNode>("menu");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    addChild(layer);
    
    _background = assets->get<scene2::SceneNode>("menu_background");
    
//    _create = false;
    _host = false;
    _join = false;
    
    _hostButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_host"));
    _hostButton->addListener([=](const std::string& name, bool down) {
        _host = true;
        NetworkController::createGame();
        _joinButton->deactivate();
        _usernameField->deactivate();
        _usernameField->setVisible(false);
//        _slider->setVisible(false);
//        _slider->deactivate();
//        _label->setVisible(false);
//        _joinButton->dispose();
        this->_active = down;
        _create = true;
    });
    
    _joinButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_join"));
    _joinButton->addListener([=](const std::string& name, bool down) {
        CULog("join button pressed");
        _host = false;
//        _slider->setVisible(false);
//        _slider->deactivate();
//        _label->setVisible(false);
        _hostButton->setVisible(false);
        _hostButton->deactivate();
        _usernameField->deactivate();
        _usernameField->setVisible(false);
        _usernameLabel->setVisible(false);
        _joinButton->setVisible(false);
        _codeField->activate();
        _codeField->setVisible(true);
        _join = true;
        _exitJoinButton->setVisible(true);
        _exitJoinButton->activate();
    });
    
    _settingsNode = std::make_shared<Settings>(assets, false);
    _settingsNode->setVisible(false);
    _settingsNode->setActive(false);
    addChild(_settingsNode);
    
    
    _settingsButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_settings"));
    _settingsButton->activate();
    _settingsButton->addListener([=](const std::string& name, bool down) {
//        CULog("settings button pressed");
        _settings = down;
        _settingsNode->setVisible(true);
        _settingsNode->setActive(true);
        _background->setColor(Color4(255,255,255,100));
        _joinButton->setVisible(false);
        _hostButton->setVisible(false);
        if (_join) {
            _codeField->deactivate();
            _codeField->setVisible(false);
        }
        _joinButton->deactivate();
        _hostButton->deactivate();
    });
    
    _exitJoinButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_exitjoin"));
    _exitJoinButton->setVisible(false);
    _exitJoinButton->addListener([=](const std::string& name, bool down) {
        _host = false;
        _hostButton->setVisible(true);
        _hostButton->activate();
        _usernameField->activate();
        _usernameField->setVisible(true);
        _usernameLabel->setVisible(true);
        _joinButton->setVisible(true);
        _joinButton->activate();
        _codeField->deactivate();
        _codeField->setVisible(false);
        _join = false;
        _exitJoinButton->setVisible(false);
    });
    
//    _slider = std::dynamic_pointer_cast<scene2::Slider>(assets->get<scene2::SceneNode>("menu_slider"));
//    _label  = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_slider_label"));
//    _label->setText("Always Forward");
//    _movement = 0;
//    _slider->addListener([=](const std::string& name, float value) {
//        if (value != _sliderValue) {
//            _sliderValue = value;
//            _movement = static_cast<int>(value);
//            switch (_movement) {
//                case 0:
//                    _label->setText("Always Forward");
//                    break;
//                case 1:
//                    _label->setText("Swipe Force");
//                    break;
//                case 2:
//                    _label->setText("Tilt to Move");
//                    break;
//                case 3:
//                    _label->setText("Golfing");
//                    break;
//                default:
//                    break;
//            }
//        }
//    });
//
//    _slider->setVisible(false);
//    _label->setVisible(false);
    
    _codeField = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_joincode"));
    _codeField->addTypeListener([=](const std::string& name, const std::string& value) {
        CULog("Change to %s", value.c_str());
        });
    _codeField->addExitListener([=](const std::string& name, const std::string& value) {
        CULog("Finish to %s", value.c_str());
        NetworkController::joinGame(value);
        //this->_active = false;
    });
    
    _usernameLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_usernamelabel"));

    _usernameField = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_username"));
    _usernameField->addTypeListener([=](const std::string& name, const std::string& value) {
        CULog("Change to %s", value.c_str());
        });
    _usernameField->addExitListener([=](const std::string& name, const std::string& value) {
        CULog("Finish to %s", value.c_str());
        NetworkController::setUsername(value);
    });
    
//    _usernameField->setPosition(dimen.width/2, dimen.height/6);
    
    Input::activate<TextInput>();
    _codeField->setVisible(false);
//    if(_active) {
//        _slider->activate();
//    }
    Application::get()->setClearColor(Color4(255,255,255,255));
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
//    removeAllChildren();
    _hostButton = nullptr;
    _joinButton = nullptr;
    _settingsButton = nullptr;
//    Input::deactivate<TextInput>();
    _codeField = nullptr;
    _usernameField = nullptr;
    _slider = nullptr;
    _assets = nullptr;
    _active = false;
    _create = false;
    _join = false;
    _settings = false;
    _settingsNode = nullptr;
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



/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void MenuScene::setActive(bool value) {
    _active = value;
//    Input::activate<TextInput>();
    if (value && (!_hostButton->isActive() || !_joinButton->isActive())) {
//        _slider->setVisible(true);
//        _label->setVisible(true);
//        _slider->activate();
        _hostButton->activate();
        _joinButton->activate();
        _hostButton->setVisible(true);
        _joinButton->setVisible(true);
        _codeField->deactivate();
        _codeField->setVisible(false);
        _usernameField->activate();
    } else if (!value && (_hostButton->isActive() || _joinButton->isActive() || !_codeField->isActive())) {
        _hostButton->deactivate();
        _joinButton->deactivate();
        _codeField->deactivate();
        _usernameField->deactivate();
        _settingsButton->deactivate();
//        _slider->deactivate();
    }
}

void MenuScene::update() {
    if (_settingsNode->backPressed()) {
        CULog("MENU: settings back pressed");
        _settings = false;
        _settingsNode->setVisible(false);
        _settingsNode->setActive(false);
        _background->setColor(Color4(255,255,255,255));
        if (_join) {
//            CULog("join true");
            _codeField->activate();
            _codeField->setVisible(true);
            _exitJoinButton->setVisible(true);
            _exitJoinButton->activate();
        }
        else {
            _joinButton->setVisible(true);
            _hostButton->setVisible(true);
            _joinButton->activate();
            _hostButton->activate();
            _usernameLabel->setVisible(true);
            _usernameField->setVisible(true);
            _usernameField->activate();
        }
    }
    
    if(_settingsNode->isVisible()) {
        CULog("MENU: settings visible");
    }
    
}
