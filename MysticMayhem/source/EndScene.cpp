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
bool EndScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::map<std::string, int> results, std::string message, 
    bool endEarly) {
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

    _hostDisconnectLabel = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_hostDisconnect"));
    _hostDisconnectLabel->setVisible(endEarly);
    _hostDisconnectLabel->setColor(cugl::Color4::RED);
    _hostDisconnectLabel->setScale(0.5);
    

    typedef std::function<bool(std::pair<std::string, int>, std::pair<std::string, int>)> Comparator;

    Comparator compFunctor =
            [](std::pair<std::string, int> elem1, std::pair<std::string, int> elem2)
            {
                return elem1.second >= elem2.second;
            };
    
    //std::set<std::pair<std::string, int>, Comparator> setPlayerScores = {};
    // sorting by the player scores is happening here
    std::set<std::pair<std::string, int>, Comparator> setPlayerScores(
            _resultsMap.begin(), _resultsMap.end(), compFunctor);
    
    _player1NameLabel= std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_playername1"));
    _player1ScoreLabel= std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_playerscore1"));
    Vec2 scorepos = _player1ScoreLabel->getPosition();
    Vec2 namepos = _player1NameLabel->getPosition();
    int i = 0;
    //setting all the player labels to be not visible at init
    for (int i = 0; i < 8; i++) {
        std::shared_ptr<cugl::scene2::Label> playerScore = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_playerscore"+std::to_string(i+1)));
        std::shared_ptr<cugl::scene2::Label> playerName = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_playername"+std::to_string(i+1)));
        playerScore->setVisible(false);
        playerName->setVisible(false);
    }
    
    int prevScore = 0;
    int prevPlace = 1;
    for (std::pair<std::string, int> element : setPlayerScores) {
//    for (const auto& [key, value] : _resultsMap) {
        if (i == 0) {
            prevScore = element.second;
            stringstream ss;
             ss << std::to_string(i+1) << ". " << element.first;
            _player1NameLabel->setText(ss.str());
            stringstream score;
            score << element.second;
            _player1ScoreLabel->setText(score.str());
            _player1ScoreLabel->setVisible(true);
            _player1NameLabel->setVisible(true);
        }
        else {
            stringstream ss;
            std::shared_ptr<cugl::scene2::Label> playerScore = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_playerscore"+std::to_string(i+1)));
            std::shared_ptr<cugl::scene2::Label> playerName = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_playername"+std::to_string(i+1)));
            playerScore->setPosition(scorepos.x, scorepos.y - (i * 30));
            playerName->setPosition(namepos.x, namepos.y - (i * 30));
            if (prevScore == element.second) {
                ss << std::to_string(prevPlace) << ". " << element.first;
            }
            else {
                prevScore = element.second;
                prevPlace = i+1;
                ss << std::to_string(i+1) << ". " << element.first;
            }
            playerName->setText(ss.str());
            stringstream score;
            score << element.second;
            playerScore->setText(score.str());
            playerScore->setVisible(true);
            playerName->setVisible(true);
        }
        i++;
    }
    
//    _message = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("end_message"));
//    _message->setText(message);

    
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
    _player1ScoreLabel = nullptr;
    _player1NameLabel = nullptr;
    _resultLabel = nullptr;
    _hostDisconnectLabel = nullptr;
    _mainMenu = false;
    _playAgain = false;
    _active = false;
    _resultsMap.clear();
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

