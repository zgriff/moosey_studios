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
#define SCENE_SIZE      1024

#define CODE_LENGTH     6
#define NUM_CODE_ICONS  7

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
//        _label->setVisible(false);
//        _joinButton->dispose();
        this->_active = down;
        _create = true;
    });
    
    _joinButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_join"));
    _joinButton->addListener([=](const std::string& name, bool down) {
        CULog("join button pressed");
        _host = false;
//        _label->setVisible(false);
        _hostButton->setVisible(false);
        _hostButton->deactivate();
        _usernameField->deactivate();
        _usernameField->setVisible(false);
        _usernameLabel->setVisible(false);
        _usernamePlate->setVisible(false);
        _joinButton->setVisible(false);
        _codeNode->setVisible(true);
        _lobbyButton->setVisible(true);
        _lobbyButton->activate();
        _deleteButton->setVisible(true);
        _deleteButton->activate();
        for (int i = 0; i < NUM_CODE_ICONS; i++) {
            _codeButtons[i]->activate();
            _codeButtons[i]->setVisible(true);
            _codeIcons[i]->setVisible(true);
        }
//        _codeField->activate();
//        _codeField->setVisible(true);
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
        _usernamePlate->setVisible(false);
        if (_join) {
            _codeNode->setVisible(false);
            _lobbyButton->setVisible(false);
            _lobbyButton->deactivate();
            _deleteButton->setVisible(false);
            _deleteButton->deactivate();
            for (int i = 0; i < NUM_CODE_ICONS; i++) {
                _codeButtons[i]->deactivate();
                _codeButtons[i]->setVisible(false);
                _codeIcons[i]->setVisible(false);
            }
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
        _usernamePlate->setVisible(true);
        _joinButton->setVisible(true);
        _joinButton->activate();
        _codeNode->setVisible(false);
        _lobbyButton->setVisible(false);
        _lobbyButton->deactivate();
        _deleteButton->setVisible(false);
        _deleteButton->deactivate();
        for (int i = 0; i < NUM_CODE_ICONS; i++) {
            _codeButtons[i]->deactivate();
            _codeButtons[i]->setVisible(false);
            _codeIcons[i]->setVisible(false);
        }
        _join = false;
        _exitJoinButton->setVisible(false);
    });
    

//    int button_offset = 100;
    for (int i = 0; i < NUM_CODE_ICONS; i++) {
        auto codeButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joincode"+to_string(i+1)));
        _codeButtons.push_back(codeButton);
        auto codeIcon = std::dynamic_pointer_cast<scene2::PolygonNode>(assets->get<scene2::SceneNode>("menu_codeicon"+to_string(i+1)));
        _codeIcons.push_back(codeIcon);
    }
    
    _codeNode = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("menu_codenode"));
    
    _codeCount = 0;
    int buttonId = 0;
    for (auto it = _codeButtons.begin(); it != _codeButtons.end(); it++) {
        (*it)->addListener([=](const std::string& name, bool down) {
            if (down && _codeCount < CODE_LENGTH) {
                auto icon = std::dynamic_pointer_cast<scene2::PolygonNode>(_codeNode->getChild(_codeCount)->getChildByName(buttonToCode(buttonId)));
                icon->setVisible(true);
                _joinCode.push_back(buttonId);
                _codeCount++;
            }
        });
        buttonId++;
    }
    
    _deleteButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_codeback"));
    _deleteButton->setVisible(false);
    _deleteButton->deactivate();
    _deleteButton->addListener([=](const std::string& name, bool down) {
        if (down && _codeCount != 0) {
            auto children = _codeNode->getChild(_codeCount-1)->getChildren();
            for (auto it = children.begin(); it != children.end(); it++) {
                if ((*it)->getName() != "label") {
                    (*it)->setVisible(false);
                }
            }
            _joinCode.pop_back();
            _codeCount--;
        }
    });
    
    _lobbyButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("menu_joinlobby"));
    _lobbyButton->setVisible(false);
    _lobbyButton->deactivate();
    _lobbyButton->addListener([=](const std::string& name, bool down) {
        if (down) {
            int value = 0;
            for (int i = 0; i < _joinCode.size(); i++) {
                value += _joinCode[i]*(pow(7,(_joinCode.size()-i-1)));
            }
            NetworkController::joinGame(to_string(value));
        }
    });
    
//    _codeField = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_joincode"));
//    _codeField->addTypeListener([=](const std::string& name, const std::string& value) {
//        CULog("Change to %s", value.c_str());
//        });
//    _codeField->addExitListener([=](const std::string& name, const std::string& value) {
//        CULog("Finish to %s", value.c_str());
//        NetworkController::joinGame(value);
//        //this->_active = false;
//    });
    
    _usernameLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("menu_usernamelabel"));
    _usernameLabel->setVisible(true);
    _usernameField = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("menu_username"));
    _usernameField->setVisible(true);
    _usernameField->addTypeListener([=](const std::string& name, const std::string& value) {
        CULog("Change to %s", value.c_str());
        });
    _usernameField->addExitListener([=](const std::string& name, const std::string& value) {
        CULog("Finish to %s", value.c_str());
        NetworkController::setUsername(value);
    });
    _usernamePlate = std::dynamic_pointer_cast<scene2::SceneNode>(assets->get<scene2::SceneNode>("menu_usernameplate"));
    _usernamePlate->setVisible(true);
    
//    _usernameField->setPosition(dimen.width/2, dimen.height/6);
    
    Input::activate<TextInput>();
//    _codeField->setVisible(false);

    Application::get()->setClearColor(Color4(255,255,255,255));
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
//    removeAllChildren();
    _codeNode = nullptr;
    _joinCode.clear();
    _codeIcons.clear();
    _codeButtons.clear();
    _deleteButton = nullptr;
    _lobbyButton = nullptr;
    _hostButton = nullptr;
    _joinButton = nullptr;
    _settingsButton = nullptr;
//    Input::deactivate<TextInput>();
//    _codeField = nullptr;
    _usernameField = nullptr;
    _assets = nullptr;
    _active = false;
    _create = false;
    _join = false;
    _settings = false;
    _settingsNode = nullptr;
}

void MenuScene::clearListeners() {
    _hostButton->clearListeners();
    _joinButton->clearListeners();
    _lobbyButton->clearListeners();
    _exitJoinButton->clearListeners();
    _settingsButton->clearListeners();
    _deleteButton->clearListeners();
    for (auto it = _codeButtons.begin(); it != _codeButtons.end(); it++) {
        (*it)->clearListeners();
    }
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
        _hostButton->activate();
        _joinButton->activate();
        _hostButton->setVisible(true);
        _joinButton->setVisible(true);
        _codeNode->setVisible(false);
        _lobbyButton->setVisible(false);
        _lobbyButton->deactivate();
        _deleteButton->setVisible(false);
        _deleteButton->deactivate();
        for (int i = 0; i < NUM_CODE_ICONS; i++) {
            _codeButtons[i]->deactivate();
            _codeButtons[i]->setVisible(false);
            _codeIcons[i]->setVisible(false);
        }
//        _codeField->deactivate();
//        _codeField->setVisible(false);
        _usernameField->activate();
    } else if (!value && (_hostButton->isActive() || _joinButton->isActive())) {
        _hostButton->deactivate();
        _joinButton->deactivate();
        _codeNode->setVisible(false);
        _lobbyButton->setVisible(false);
        _lobbyButton->deactivate();
        _deleteButton->setVisible(false);
        _deleteButton->deactivate();
        for (int i = 0; i < NUM_CODE_ICONS; i++) {
            _codeButtons[i]->deactivate();
            _codeButtons[i]->setVisible(false);
            _codeIcons[i]->setVisible(false);
        }
        for (int i = 0; i < (int)_codeNode->getChildren().size(); i++) {
            auto nodeChild = _codeNode->getChild(i)->getChildren();
            for (auto it = nodeChild.begin(); it != nodeChild.end(); it++) {
                if ((*it)->getName() != "label") {
                    (*it)->setVisible(false);
                }
            }
        }
//        _codeField->deactivate();
        _usernameField->deactivate();
        _settingsButton->deactivate();
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
            _codeNode->setVisible(true);
            _lobbyButton->setVisible(true);
            _lobbyButton->activate();
            _deleteButton->setVisible(true);
            _deleteButton->activate();
            for (int i = 0; i < NUM_CODE_ICONS; i++) {
                _codeButtons[i]->activate();
                _codeButtons[i]->setVisible(true);
                _codeIcons[i]->setVisible(true);
            }
//            _codeField->activate();
//            _codeField->setVisible(true);
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
            _usernamePlate->setVisible(true);
            _usernameField->activate();
        }
    }
    
    if(_settingsNode->isVisible()) {
        CULog("MENU: settings visible");
    }
    
}


std::string MenuScene::buttonToCode(int button) {
    std::string result = "";
    switch (button) {
        case 0:
            result = "fire";
            break;
        case 1:
            result = "water";
            break;
        case 2:
            result = "leaf";
            break;
        case 3:
            result = "egg";
            break;
        case 4:
            result = "orb";
            break;
        case 5:
            result = "tree";
            break;
        case 6:
            result = "bush";
            break;
        default:
            break;
    }
    return result;
}
