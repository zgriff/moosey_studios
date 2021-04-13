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

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

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
    
    _startButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("lobby_start"));
    
//    if(NetworkController::isHost()){
//        _startButton->addListener([&](const std::string& name, bool down) {
//            if(true){ //TODO: if everyone ready
//                NetworkController::startGame();
//                _active = down;
//            }
//        });
//    }else{
//        _startButton->addListener([&](const std::string& name, bool down) {
//            if(clientReady){
//                NetworkController::unready();
//            }else{
//                NetworkController::ready();
//            }
//            clientReady = !clientReady;
//        });
//    }
    _startButton->addListener([&](const std::string& name, bool down) {
        if(true){ //TODO: if everyone ready
            _active = down;
        }
    });
    
    _roomId = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("lobby_roomId"));
    
    
    auto kids = assets->get<scene2::SceneNode>("lobby_players")->getChildren();
    for(auto it = kids.begin(); it != kids.end(); ++it) {
        std::shared_ptr<scene2::Label> label = std::dynamic_pointer_cast<scene2::Label>(*it);
        _playerLabels.push_back(label);
        label->setVisible(false);
    }
    
    
    Input::activate<TextInput>();
    if(_active) {
//        _slider->activate();
    }

    Application::get()->setClearColor(Color4(192,192,192,255));
//    NetworkController::setStartCallback([&](){
//        CULog("called !!!!!");
//        this->setActive(false);
//    });
//    NetworkController::setReadyCallback([&](uint8_t playerid, bool r){
//        return;
//    });
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LobbyScene::dispose() {
    _startButton = nullptr;
    _playerLabels.clear();
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
void LobbyScene::update(float progress) {
//    NetworkController::update(progress);
    NetworkController::step();
    if (_currRoomId == "") {
        _currRoomId = NetworkController::getRoomId();
        stringstream ss;
        ss << "Room Id: " << _currRoomId;
        _roomId->setText(ss.str());
    }
    std::string s = NetworkController::getRoomId();
    
    for (auto it = _playerLabels.begin(); it!=_playerLabels.end();it++) {
        std::string  name = (*it)->getName();
        name = name.back();
        int player_num = std::stoi(name);
        
        if (player_num <= NetworkController::getNumPlayers()) {
            (*it)->setVisible(true);
        }
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
    } else {
        _startButton->deactivate();
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
