//
//  MatchmakingScene.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/25/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "LobbyScene.h"
#include <time.h>
#include <string>
#include "Settings.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

float PLAYER_POSITION[] = {2.1f,  1.0f};

bool clientReady;

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
bool LobbyScene::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    clientReady = false;
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
    auto layer = assets->get<scene2::SceneNode>("lobby");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    addChild(layer);
    _layer = layer;
    
    
    
    _startButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_start"));
    _startButton->setVisible(false);
    _map1Button = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_maps_map1"));
    _map2Button = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_maps_map2"));
    _map3Button = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_maps_map3"));
    _map4Button = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_maps_map4"));
    _map1Button->setToggle(true);
    _map2Button->setToggle(true);
    _map3Button->setToggle(true);
    _map4Button->setToggle(true);
    _map1Button->setDown(false);
    _map2Button->setDown(false);
    _map3Button->setDown(false);
    _map4Button->setDown(false);
    
    
    _mapNextButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_maps_mapnext"));
    _mapPrevButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_maps_mapprev"));
    _mapNextButton->setVisible(false);
    _mapPrevButton->setVisible(false);
    _mapNextButton->activate();
    _mapPrevButton->activate();
    
    
    
    if(NetworkController::isHost()){
        _startButton->setVisible(true);
        _startButton->addListener([&](const std::string& name, bool down) {
            if(true){ //TODO: if everyone ready
                NetworkController::sendSetMapSelected(NetworkController::getMapSelected());
                NetworkController::startGame();
                _active = down;
            }
        });
        
        //TODO: extend these to not use mod 4
        _mapNextButton->setVisible(true);
        _mapNextButton->addListener([&](const std::string& name, bool down) {
            if (down) {
                int map = NetworkController::getMapSelected()%4 +  1;
                NetworkController::sendSetMapSelected(map);
                NetworkController::setMapSelected(map);
            }
        });
        
        _mapPrevButton->setVisible(true);
        _mapPrevButton->addListener([&](const std::string& name, bool down) {
            if (down) {
                int map = NetworkController::getMapSelected() - 1;
                if (map == 0) {
                    map = 4;
                }
                NetworkController::sendSetMapSelected(map);
                NetworkController::setMapSelected(map);
            }
        });
        
//        _map1Button->setVisible(true);
//        _map1Button->addListener([&](const std::string& name, bool down) {
//            if (_map1Button->isDown()) {
//                _map2Button->setDown(false);
//                _map3Button->setDown(false);
//                _map4Button->setDown(false);
//                /*_map1Button->deactivate();
//                _map2Button->activate();
//                _map3Button->activate();
//                _map4Button->activate();*/
//                _selectedMap = GRASS_MAP_KEY;
//                NetworkController::sendSetMapSelected(1);
//                NetworkController::setMapSelected(1);
//                CULog("%f %f", _map1Button->getPositionX(), _map1Button->getPositionY());
//            }
//        });
//        _map2Button->setVisible(true);
//        _map2Button->addListener([&](const std::string& name, bool down) {
//            if (_map2Button->isDown()) {
//                _map1Button->setDown(false);
//                _map3Button->setDown(false);
//                _map4Button->setDown(false);
//                /*_map1Button->activate();
//                _map2Button->deactivate();
//                _map3Button->activate();
//                _map4Button->activate();*/
//                NetworkController::sendSetMapSelected(2);
//                NetworkController::setMapSelected(2);
//                _selectedMap = GRASS_MAP2_KEY;
//            }
//        });
//        _map3Button->setVisible(true);
//        _map3Button->addListener([&](const std::string& name, bool down) {
//            if (_map3Button->isDown()) {
//                _map1Button->setDown(false);
//                _map2Button->setDown(false);
//                _map4Button->setDown(false);
//                /*_map1Button->activate();
//                _map2Button->activate();
//                _map3Button->deactivate();
//                _map4Button->activate();*/
//                NetworkController::sendSetMapSelected(3);
//                NetworkController::setMapSelected(3);
//                _selectedMap = GRASS_MAP3_KEY;
//            }
//            });
//        _map4Button->setVisible(true);
//        _map4Button->addListener([&](const std::string& name, bool down) {
//            if (_map4Button->isDown()) {
//                _map1Button->setDown(false);
//                _map2Button->setDown(false);
//                _map3Button->setDown(false);
//               /* _map1Button->activate();
//                _map2Button->activate();
//                _map3Button->activate();
//                _map4Button->deactivate();*/
//                NetworkController::sendSetMapSelected(4);
//                NetworkController::setMapSelected(4);
//                _selectedMap = GRASS_MAP4_KEY;
//            }
//            });
    }
    else{
//        _startButton->addListener([&](const std::string& name, bool down) {
//            if(clientReady){
//                NetworkController::unready();
//            }else{
//                NetworkController::ready();
//            }
//            clientReady = !clientReady;
//        });
//        _map1Button->deactivate();
//        _map1Button->setVisible(false);
//        _map2Button->deactivate();
//        _map2Button->setVisible(false);
//        _map3Button->deactivate();
//        _map3Button->setVisible(false);
//        _map4Button->deactivate();
//        _map4Button->setVisible(false);
    }
    
    _settingsNode = std::make_shared<Settings>(assets, false);
    _settingsNode->setVisible(false);
    _settingsNode->setActive(false);
    addChild(_settingsNode);
    
    _settingsButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_settings"));
    _settingsButton->activate();
    _settingsButton->addListener([=](const std::string& name, bool down) {
        _settingsNode->setVisible(true);
        _settingsNode->setActive(true);
        layer->setColor(Color4(255,255,255,100));
        _map1Button->deactivate();
        _map2Button->deactivate();
        _map3Button->deactivate();
        _map4Button->deactivate();
        _startButton->deactivate();
        if(_settingsNode->isVisible()) {
            CULog("LOBBY: settings visible");
        }
    });

    
    _roomId = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_roomId"));
    
    
    auto kids = assets->get<scene2::SceneNode>("lobby_players")->getChildren();
    for(auto it = kids.begin(); it != kids.end(); ++it) {
        std::shared_ptr<scene2::Label> label = std::dynamic_pointer_cast<scene2::Label>(*it);
        _playerLabels.push_back(label);
        label->setVisible(false);
    }
    
    
    Vec2 playerPos = ((Vec2)PLAYER_POSITION);
    Size playerSize(4, 8);
    _playerCustom = Player::alloc(playerPos, playerSize, Element::Water);
    _playerCustom->setSkinKey("player_skin");
    _playerCustom->setColorKey("player_color");
    _playerCustom->setFaceKey("player_face");
    _playerCustom->setBodyKey("player_body_line");
    _playerCustom->setHatKey("player_hat");
    _playerCustom->setStaffKey("player_staff");
    _playerCustom->setStaffTagKey("player_staff_tag");
    _playerCustom->setRingKey("player_direction");
    _playerCustom->setTextures(_assets);
    _playerCustom->setDrawScale(100.0f);
    _playerCustom->flipHorizontal(false);
    _playerCustom->setSkin(1);
    _playerCustom->getSceneNode()->setScale(0.25f);
    addChild(_playerCustom->getSceneNode(),1);
    
    NetworkController::sendSetCustomization( NetworkController::getPlayerId().value(), _playerCustom->getSkin(), _playerCustom->getCustomization(), _playerCustom->getCurrElement());
    
    _hatForwardButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_customization_hatforw"));
    _hatBackButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_customization_hatback"));
    _skinForwardButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_customization_skinforw"));
    _skinBackButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_customization_skinback"));
    _eleForwardButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_customization_eleforw"));
    _eleBackButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_customization_eleback"));
    
    
    
    _hatForwardButton->activate();
    _hatBackButton->activate();
    _skinForwardButton->activate();
    _skinBackButton->activate();
    _eleForwardButton->activate();
    _eleBackButton->activate();
    
    _hatForwardButton->setScale(0.2f);
    _hatBackButton->setScale(0.2f);
    _skinForwardButton->setScale(0.2f);
    _skinBackButton->setScale(0.2f);
    _eleForwardButton->setScale(0.2f);
    _eleBackButton->setScale(0.2f);
    
    _hatForwardButton->addListener([=](const std::string& name, bool down) {
        CULog("hat forw");
        if (down) {
            _playerCustom->setCustomization((_playerCustom->getCustomization()+1)%6);
            _playerCustom->setDrawScale(100.0f);
        }
    });
    
    _hatBackButton->addListener([=](const std::string& name, bool down) {
        CULog("hat back");
        if (down) {
            if (_playerCustom->getCustomization() == 0) {
                _playerCustom->setCustomization(5);
                _playerCustom->setDrawScale(100.0f);
            } else {
                _playerCustom->setCustomization((_playerCustom->getCustomization()-1));
                _playerCustom->setDrawScale(100.0f);
            }
            NetworkController::sendSetCustomization( NetworkController::getPlayerId().value(), _playerCustom->getSkin(), _playerCustom->getCustomization(), _playerCustom->getCurrElement());
        }
        
    });
    
    _skinForwardButton->addListener([=](const std::string& name, bool down) {
        CULog("skin forw");
        if (down) {
            _playerCustom->setSkin((_playerCustom->getSkin()+1)%2);
            _playerCustom->setDrawScale(100.0f);
            NetworkController::sendSetCustomization( NetworkController::getPlayerId().value(), _playerCustom->getSkin(), _playerCustom->getCustomization(), _playerCustom->getCurrElement());
        }
    });
    
    _skinBackButton->addListener([=](const std::string& name, bool down) {
        CULog("skin back");
        if (down) {
            _playerCustom->setSkin((_playerCustom->getSkin()+1)%2);
            _playerCustom->setDrawScale(100.0f);
            NetworkController::sendSetCustomization( NetworkController::getPlayerId().value(), _playerCustom->getSkin(), _playerCustom->getCustomization(), _playerCustom->getCurrElement());
        }
    });
    
    _eleForwardButton->addListener([=](const std::string& name, bool down) {
        CULog("ele forw");
        if (down) {
            _playerCustom->setElement(_playerCustom->getPreyElement());
            _playerCustom->setElement(_playerCustom->getPreyElement());
            _playerCustom->setDrawScale(100.0f);
            NetworkController::sendSetCustomization( NetworkController::getPlayerId().value(), _playerCustom->getSkin(), _playerCustom->getCustomization(), _playerCustom->getCurrElement());
        }
    });
    
    _eleBackButton->addListener([=](const std::string& name, bool down) {
        CULog("ele back");
        if (down) {
            _playerCustom->setElement(_playerCustom->getPreyElement());
            _playerCustom->setDrawScale(100.0f);
            NetworkController::sendSetCustomization( NetworkController::getPlayerId().value(), _playerCustom->getSkin(), _playerCustom->getCustomization(), _playerCustom->getCurrElement());
        }
        
    });
    
    
    
    
    Input::activate<TextInput>();
    if(_active) {
//        _slider->activate();
    }

    Application::get()->setClearColor(Color4(192,192,192,255));
    NetworkController::setStartCallback([&](){
        CULog("called !!!!!");
        _active = false;
    });
//    NetworkController::setReadyCallback([&](uint8_t playerid, bool r){
//        return;
//    });
    _playAgain = false;
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LobbyScene::dispose() {
//    removeAllChildren();
    _startButton = nullptr;
    _map1Button = nullptr;
    _map2Button = nullptr;
    _map3Button = nullptr;
    _map4Button = nullptr;
    _settingsButton = nullptr;
    _hatForwardButton = nullptr;
    _hatBackButton = nullptr;
    _skinForwardButton = nullptr;
    _skinBackButton = nullptr;
    _eleForwardButton = nullptr;
    _eleBackButton = nullptr;
    _mapNextButton = nullptr;
    _mapPrevButton = nullptr;
    _playerLabels.clear();
    _assets = nullptr;
    _active = false;
    _playAgain = false;
    _currRoomId = "";
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
void LobbyScene::update(float progress) {
//    NetworkController::update(progress);
    NetworkController::step();
    if (_currRoomId == "") {
        _currRoomId = NetworkController::getRoomId();
//        CULog("room id in lobby %s", _currRoomId.c_str());
        stringstream ss;
        ss << "Room Id: " << _currRoomId;
        _roomId->setText(ss.str());
    }
    std::string s = NetworkController::getRoomId();
    
    for (auto it = _playerLabels.begin(); it!=_playerLabels.end();it++) {
        std::string  name = (*it)->getName();
        name = name.back();
        char nullChar = 0;
        if (name[0] != nullChar) {
            int player_num = std::stoi(name);

            if (player_num <= NetworkController::getNumPlayers()) {
                /*if (player_num == 1) {
                    char nullChar = 0;
                    std::string testUsername = "testname2";
                    testUsername += nullChar;
                    CULog("username with null is %s", testUsername.c_str());
                    NetworkController::sendSetUsername(player_num, testUsername);
                }*/
                //NetworkController::sendSetUsername(player_num, "testname");
                /*CULog("setting username label text %s for player %d",
                    NetworkController::getUsername(it - _playerLabels.begin()).c_str(), it - _playerLabels.begin());*/
                NetworkController::setUsername(NetworkController::getUsername(), NetworkController::getPlayerId().value());
                char nullChar = 0;
                NetworkController::sendSetUsername(NetworkController::getPlayerId().value(), NetworkController::getUsername() + nullChar);
                (*it)->setText(NetworkController::getUsername(it - _playerLabels.begin()));
                (*it)->setVisible(true);
            }
        }
    }

//    if (!NetworkController::isHost()) {
    if (NetworkController::getMapSelected() == 1) {
        _map1Button->setVisible(true);
        _map1Button->setPosition(550, 100);
        _map2Button->setVisible(false);
        _map3Button->setVisible(false);
        _map4Button->setVisible(false);
    }
    else if (NetworkController::getMapSelected() == 2) {
        _map2Button->setVisible(true);
        _map2Button->setPosition(550, 100);
        _map1Button->setVisible(false);
        _map3Button->setVisible(false);
        _map4Button->setVisible(false);
    }
    else if (NetworkController::getMapSelected() == 3) {
        _map3Button->setVisible(true);
        _map3Button->setPosition(550, 100);
        _map1Button->setVisible(false);
        _map2Button->setVisible(false);
        _map4Button->setVisible(false);
    }
    else if (NetworkController::getMapSelected() == 4) {
        
        _map4Button->setVisible(true);
        _map4Button->setPosition(550, 100);
        _map1Button->setVisible(false);
        _map2Button->setVisible(false);
        _map3Button->setVisible(false);
    }
//    }
    
    if (_settingsNode->backPressed()) {
        _settingsNode->setActive(false);
        _settingsNode->setVisible(false);
        _layer->setColor(Color4(255,255,255,255));
        _map1Button->activate();
        _map2Button->activate();
        _map3Button->activate();
        _map4Button->activate();
        _startButton->activate();
    }

}



/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void LobbyScene::setActive(bool value) {
    _active = value;
    if (value && !_startButton->isActive()) {
        _startButton->activate();
        _map1Button->activate();
        _map2Button->activate();
        _map3Button->activate();
        _map4Button->activate();
    } else {
        _startButton->deactivate();
        _map1Button->deactivate();
        _map2Button->deactivate();
        _map3Button->deactivate();
        _map4Button->deactivate();
        _mapNextButton->deactivate();
        _mapPrevButton->deactivate();
        _settingsButton->deactivate();
        _hatForwardButton->deactivate();
        _hatBackButton->deactivate();
        _skinForwardButton->deactivate();
        _skinBackButton->deactivate();
        _eleForwardButton->deactivate();
        _eleBackButton->deactivate();
        
    }
//    if (value && (!_hostButton->isActive() || !_joinButton->isActive())) {
//        _hostButton->activate();
//        _joinButton->activate();
//    } else if (!value && (_hostButton->isActive() || _joinButton->isActive())) {
//        _hostButton->deactivate();
//        _joinButton->deactivate();
//        _codeField->deactivate();
//    }
}
