//
//  EndScene.cpp
//  Roshamboogie
//
//  Created by Ikra Monjur on 4/3/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "EndScene.h"
#include "NetworkController.h"
#include "World.h"
#include <string>

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
bool EndScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::map<std::string, int> results, std::string message) {
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
    
    _mainMenu = false;
    _playAgain = false;
    _resultsMap = results;
    
//    _playAgainButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("end_playagain"));
////    _playAgainButton->activate();
//    _playAgainButton->addListener([=](const std::string& name, bool down) {
//        this->_active = down;
//        _playAgain = true;
//        if (NetworkController::isHost()) {
////            NetworkController::joinGame(NetworkController::getRoomId());
//            NetworkController::setPlayAgain(true);
//            NetworkController::sendPlayAgain(true);
//        }
//        else {
//            if (NetworkController::getPlayAgain()) { //if host pressed play again - can enter room
//                NetworkController::joinGame(NetworkController::getRoomId());
//            }
//        }
//    });
//    
//    if (_playAgainButton->isDown()) {
//        _playAgainButton->setDown(false);
//    }
    
    _mainMenuButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("end_mainmenu"));
    _mainMenuButton->activate();
    _mainMenuButton->addListener([=](const std::string& name, bool down) {
        this->_active = down;
        _mainMenu = true;
    });
    
    if (_mainMenuButton->isDown()) {
        _mainMenuButton->setDown(false);
    }

    _resultLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_results"));
    _resultLabel->setText("Results:");
    

    typedef std::function<bool(std::pair<std::string, int>, std::pair<std::string, int>)> Comparator;

    Comparator compFunctor =
            [](std::pair<std::string, int> elem1 ,std::pair<std::string, int> elem2)
            {
                return elem1.second >= elem2.second;
            };
    
    // sorting by the player scores is happening here
    std::set<std::pair<std::string, int>, Comparator> setPlayerScores(
            _resultsMap.begin(), _resultsMap.end(), compFunctor);
    
    Vec2 pos = _resultLabel->getPosition();
    int i = 1;
    for (std::pair<std::string, int> element : setPlayerScores) {
//    for (const auto& [key, value] : _resultsMap) {
        stringstream ss;
        std::shared_ptr<cugl::scene2::Label> playerScore = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_playerscore"+std::to_string(i)));
        playerScore->setPosition(pos.x, pos.y - (i * 30));
        ss << element.first << " SCORE: " << element.second;
        playerScore->setText(ss.str());
        i++;
    }
    
    _message = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_message"));
    _message->setText(message);

    
    Application::get()->setClearColor(Color4(190,187,191,255));
    return true;
}

/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void EndScene::dispose() {
    CULog("Results dispose");
    removeAllChildren();
    _assets = nullptr;
    _playAgainButton = nullptr;
    _mainMenuButton = nullptr;
    _mainMenu = false;
    _playAgain = false;
    _active = false;
    Scene2::dispose();
    
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

