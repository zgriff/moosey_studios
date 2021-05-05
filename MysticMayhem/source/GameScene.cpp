//
//  GameScene.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "GameScene.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "Element.h"
#include "NetworkController.h"
#include "NetworkData.h"
#include "CollisionController.h"
#include "AbilityController.h"
#include "MapConstants.h"

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <cmath>

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_WIDTH 1440
#define SCENE_HEIGHT 720

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   36.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f
/** The restitution for all physics objects */
#define TURNS_PER_SPIN   55.0f
/** how much the sideways velocity is subtracted per frame
    1.0 decelerates turning player to 14.6 velocity*/
#define KINETIC_FRICTION 2.5f
/** how much the backwards velocity is subtracted per frame if the player is going backwards*/
#define BACKWARDS_FRICTION 0.5f
/** how quickly the camera catches up to the player*/
#define CAMERA_STICKINESS .07f
/** How much the camera */
#define CAMERA_ZOOM 0.65f
/** baseline aspect ratio, 1468.604 is from 1280x720 */
#define BASELINE_DIAGONAL 1468.60478005
#define BASELINE_HEIGHT 720 //if we want to scale by height instead just change the places w/ length and diagonal to height

#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets, string mapKey) {
    // Initialize the scene to a locked width
    //create world
    CULog("map selected is %d", NetworkController::getMapSelected());
    switch (NetworkController::getMapSelected()) {
    case 1:
        mapKey = GRASS_MAP_KEY;
        break;
    case 2:
        mapKey = GRASS_MAP2_KEY;
        break;
    case 3:
        mapKey = GRASS_MAP3_KEY;
        break;
    case 4:
        mapKey = GRASS_MAP4_KEY;
        break;
    }
    _world = assets->get<World>(mapKey);
    if (_world == nullptr) {
        CULog("Fail!");
        return false;
    }
    
    //these represent the dimensions of the game world in scene units
    float w = _world->getSceneSize().x;
    float h = _world->getSceneSize().y;
    CULog("w is %f h is %f", w, h);
    
    Size dimen = computeActiveSize(w,h);

    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    
    NetworkController::setWorld(_world);
    _world->setNumPlayers(NetworkController::getNumPlayers());

    SpawnController::setWorld(_world);
    
    // Start up the input handler
    _assets = assets;
    _playerController.init();
    
    auto world = _world->getPhysicsWorld();
    world->activateCollisionCallbacks(true);
    _scale = dimen.width == w ? dimen.width/world->getBounds().getMaxX() : dimen.height/world->getBounds().getMaxY();
    if(NetworkController::isHost()){
        world->onBeginContact = [this](b2Contact* contact) {
            CollisionController::hostBeginContact(contact);
        };
        world->onEndContact = [this](b2Contact* contact) {
            CollisionController::endContact(contact);
        };
    }else{
        world->onBeginContact = [this](b2Contact* contact) {
            CollisionController::clientBeginContact(contact);
        };
    }
    world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        CollisionController::beforeSolve(contact,oldManifold);
    };
    
    //TODO: Change from hardcoded 8.0, figure out actual offset for phones
    _worldOffset = Vec2((dimen.width-w)/8.0f,(dimen.height-h)/2.0f);
    CULog("world off x: %f y: %f", _worldOffset.x, _worldOffset.y);
    
    _rootnode = scene2::SceneNode::alloc();
    _rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _rootnode->setPosition(_worldOffset);
    _rootnode->setContentSize(Size(w,h));

    _UInode = _assets->get<scene2::SceneNode>("ui");
    _UInode->setAnchor(Vec2::ANCHOR_CENTER);
    _UInode->setPosition(_worldOffset);
    _UInode->setContentSize(Size(w,h));
    _UInode->doLayout(); // Repositions the HUD;
    //Basically you want the inverse of camera zoom for the UInode scale, originally this wasn't set to the inverse
    //so I had to hardcode 0.4 to compensate for how the UInode childen are currently set
    _UInode->setScale(0.4/((double) CAMERA_ZOOM * ((Vec2)Application::get()->getDisplaySize()).length() / BASELINE_DIAGONAL));
    
    _scoreHUD  = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("ui_score"));
    _framesHUD = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("ui_frames"));
    _timerHUD  = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("ui_timer"));
    
    _hatchbar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("ui_bar"));
    _hatchbar->setVisible(false);
    
    _hatchnode = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("ui_hatched"));
    _hatchnode->setVisible(false);
    _startTime = time(NULL);

    _abilityname = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("ui_abilityBar_abilityName"));
    _abilityname->setVisible(false);

    _abilitybar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("ui_abilityBar"));
    _abilitybar->setForegroundColor(Color4(255, 255, 255, 100));
    
    
    _debugnode = _world->getDebugNode();
    
    addChild(_rootnode);
    addChild(_UInode);
    
    _world->setAssets(_assets);

    reset();
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    CULog("game scene dispose");
    removeAllChildren();
    _debugnode = nullptr;
    _camera = nullptr;
    _UInode = nullptr;
    _scoreHUD = nullptr;
    _hatchnode = nullptr;
    _hatchbar = nullptr;
    _abilitybar = nullptr;
    _abilityname = nullptr;
    _timerHUD = nullptr;
    _framesHUD = nullptr;
    _debug = false;
    _assets = nullptr;
//        Scene2::dispose();
    if (_world != nullptr) {
        _world->clearRootNode();
    //    _world->getSceneNode()->removeAllChildren();
        _world->getPhysicsWorld()->clear();
        _world = nullptr;
    }
    _active = false;
    _rootnode = nullptr;
    _playerController.dispose();
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    prevTime = time(NULL);
    _world->setEggSpawnCooldown(time(NULL));
    _world->setRootNode(_rootnode,_scale);
    CollisionController::setWorld(_world);
    NetworkController::setWorld(_world);
    
    auto idopt = NetworkController::getPlayerId();
    if(idopt.has_value()){
        CULog("playerid: %i",idopt.value());
        auto _player = _world->getPlayer(idopt.value());
        _player->setUsername(NetworkController::getUsername());
        _player->setIsLocal(true);
        static_pointer_cast<cugl::OrthographicCamera>(getCamera())->set(Application::get()->getDisplaySize());
        float cameraZoom = (double)CAMERA_ZOOM * ((Vec2)Application::get()->getDisplaySize()).length() / BASELINE_DIAGONAL;
        static_pointer_cast<cugl::OrthographicCamera>(getCamera())->setZoom(cameraZoom);
        auto cameraInitPlayerPos = _player->getSceneNode()->getPosition();
        //cameraZoom = 1;
        //cameraInitPlayerPos.x = std::clamp(cameraInitPlayerPos.x, Application::get()->getDisplaySize().width / 2 / cameraZoom,
        //    _world->getSceneSize().x - (Application::get()->getDisplaySize().width / 2 / cameraZoom) + 300); //right side clamp not work
        //cameraInitPlayerPos.y = std::clamp(cameraInitPlayerPos.y, Application::get()->getDisplaySize().height / 2 / cameraZoom,
        //    _world->getSceneSize().y - (Application::get()->getDisplaySize().height / 2 / cameraZoom));
        getCamera()->translate(cameraInitPlayerPos - getCamera()->getPosition());
    }
    _playerController.init();
    
    _world->setDebug(false);

    getCamera()->update();
}

void GameScene::update(float timestep) {
    
    if (_playerController.didDebug()) { _world->setDebug(!_world->getDebug()); }
    
    // NETWORK //
    
    /*std::string currRoomId = NetworkController::getRoomId();
    _roomIdHUD->setText(currRoomId);*/
//    NetworkController::step();
    NetworkController::update(timestep);

//    if (_currRoomId == "") {
//        _currRoomId = NetworkController::getRoomId();
//        stringstream ss;
//        ss << "Room Id: " << _currRoomId;
//        _roomIdHUD->setText(ss.str());
//    }
        


        // BEGIN PLAYER MOVEMENT //
    
    auto playerId_option = NetworkController::getPlayerId();
    if(! playerId_option.has_value()) return;
    uint8_t playerId = playerId_option.value();
    auto _player = _world->getPlayer(playerId);
    
    _playerController.readInput();
    switch (_playerController.getMoveStyle()) {
        case Movement::AlwaysForward: {
            if (_abilityController.getActiveAbility() == AbilityController::Ability::SpeedBoost) {
                break;
            }

            auto ang = _player->getDirection() + _playerController.getMov().x * -2.0f * M_PI * timestep * 60.0 / TURNS_PER_SPIN;
            _player->setDirection(ang > M_PI ? ang - 2.0f * M_PI : (ang < -M_PI ? ang + 2.0f * M_PI : ang));

            auto vel = _player->getLinearVelocity();
            //Please don't delete this comment, angles were difficult to derive and easy to forget
            //vel angle originates from x axis, player angle orginates from y axis
            auto offset = vel.getAngle() - _player->getDirection() + M_PI / 2.0f;
            offset = offset > M_PI ? offset - 2.0f * M_PI : (offset < -M_PI ? offset + 2.0f * M_PI : offset);

            //applies sideways friction, capped at a flat value
            auto correction = _player->getLinearVelocity().rotate(-1.0f * offset - M_PI / 2.0f).scale(sin(offset) * _player->getMass());
            if (correction.length() > KINETIC_FRICTION) {
                correction.scale(KINETIC_FRICTION / correction.length());
            }

            //apply friction if going backwards IE braking
            if (abs(offset) < M_PI / 2.0) {
                auto backwards = _player->getLinearVelocity().rotate(-1.0f * offset).scale(-1.0f * cos(offset) * _player->getMass());
                if (backwards.length() > BACKWARDS_FRICTION) {
                    backwards.scale(BACKWARDS_FRICTION / backwards.length());
                }
                _player->getBody()->ApplyLinearImpulseToCenter(b2Vec2(backwards.x, backwards.y), true);
            }
            _player->getBody()->ApplyLinearImpulseToCenter(b2Vec2(correction.x, correction.y), true);

            if (_playerController.getMov().x == 0) {

                //accelerate to a maximum velocity
                auto forForce = _player->getForce();
                auto scaling = _player->getForce();

                scaling.normalize().scale(_player->getMass() * 0.27f * pow(max(25.0f - vel.length(), 0.0f), 1.5f));

                _player->setForce(scaling);
                _player->applyForce();
                _player->setForce(forForce);
            }
            else {
                auto forForce = _player->getForce();
                auto turnForce = _player->getForce().getPerp().normalize().scale(pow(vel.length(), 1.0) * cos(offset) * 0.9f * _player->getMass() * tan(M_PI / TURNS_PER_SPIN) / timestep);
                if (_playerController.getMov().x < 0) {
                    turnForce.scale(-1.0f);
                }
                if (offset < M_PI / 2.0f && offset > -M_PI / 2.0f) {
                    //turnForce.scale(-1.0f);
                    _player->applyForce();
                }
                _player->setForce(turnForce);
                _player->applyForce();
                _player->setForce(forForce);
            }

            break;
        }
        case Movement::SwipeForce: {
            #ifndef CU_MOBILE
                _player->setLinearVelocity(_playerController.getMov() * 3);
            #else
                Vec2 moveVec = _playerController.getMoveVec();
                Vec2 _moveVec(moveVec.x, -moveVec.y);
                _player->setForce(_moveVec * 30);
                _player->applyForce();
            #endif
            break;
        }
        case Movement::TiltMove:{
            #ifndef CU_MOBILE
                _player->setLinearVelocity(_playerController.getMov() * 3);
            #else
                Vec3 tilt = _playerController.getTiltVec();

                Vec2 moveVec(tilt.x, -tilt.y);
                _player->setForce(moveVec * 50);

                _player->applyForce();
            #endif
        }
        case Movement::GolfMove:{
            #ifndef CU_MOBILE
                _player->setLinearVelocity(_playerController.getMov() * 3);
            #else
                Vec2 _moveVec;
                if (_playerController.getMov().x == 0) {
                    _moveVec = Vec2(-5*_player->getVX(),-5*_player->getVY());
                } else  {
                    Vec2 moveVec = _playerController.getMoveVec();
                    _moveVec = Vec2(moveVec.x, -moveVec.y);
                    _moveVec =  _moveVec*10;
                }
                _player->setForce(_moveVec);
                _player->applyForce();
            #endif
        }
        default:
            break;
    }
    
    _world->getPhysicsWorld()->update(timestep);


    auto playPos = _player->getSceneNode()->getPosition();
    playPos += _player->getLinearVelocity().scale(40.0 / pow(max(_player->getLinearVelocity().length(), .000001f), .35));
    auto camSpot = getCamera()->getPosition();

    ////playPos is only used for camera to calculate camera translation, we clamp it so camera doesn't display offmap
    //float cameraZoom = (double)CAMERA_ZOOM * ((Vec2)Application::get()->getDisplaySize()).length() / BASELINE_DIAGONAL;
    //playPos.x = std::clamp(playPos.x, Application::get()->getDisplaySize().width / 2 / cameraZoom,
    //    _world->getSceneSize().x - (Application::get()->getDisplaySize().width / 2 / cameraZoom) + 300); //right side clamp not work
    ///*CULog("right boundary should be %f", _world->getSceneSize().x);
    //CULog("should be cutoff at %f", _world->getSceneSize().x - (Application::get()->getDisplaySize().width / 2 / cameraZoom));
    //CULog("player pos x %f", _player->getSceneNode()->getPosition().x);*/
    //playPos.y = std::clamp(playPos.y, Application::get()->getDisplaySize().height / 2 / cameraZoom,
    //    _world->getSceneSize().y - (Application::get()->getDisplaySize().height / 2 / cameraZoom));
    ////CULog("viewport x %f", getCamera()->getViewport().size.width);

    auto trans = (playPos - camSpot)*CAMERA_STICKINESS;
    //CULog("camera x: %f camera y: %f", getCamera()->getPosition().x, getCamera()->getPosition().y);
    getCamera()->translate(trans);
    getCamera()->update();
    _UInode->setPosition(camSpot + trans - _worldOffset);

//    CULog("TIME %ld", time(NULL) - prevTime);
    if(NetworkController::isHost()){
        if (time(NULL) - prevTime >= 1) {
            //orb spawn
            int orbSpawnProb = rand() % 100;
            int randNum = rand() % 3 + 1;
            if (orbSpawnProb < 25) {
                if (_world->getCurrOrbCount() < _world->getInitOrbCount() && _world->getOrbSpawns().size() > randNum) {
                    SpawnController::spawnOrbs();
                }
            }
            
            //egg spawn
            int eggSpawnProb = rand() % 100;
            if (eggSpawnProb < 40) {
                int maxEggs = ceil((float)_world->getPlayers().size()/3);
//                CULog("max eggs %d",maxEggs);
                if (_world->getCurrEggCount() < maxEggs && time(NULL) - _world->getEggSpawnCooldown() >= 5) {
                    SpawnController::spawnEggs();
                }
            }
            prevTime = time(NULL);
        }
    }
    
    //egg hatch logic
    if (_player->getJustHitByProjectile()) {
        _player->setJustHitByProjectile(false);
        auto _egg = _world->getEgg(_player->getEggId());
        _egg->setPosition(_egg->getInitPos());
        NetworkController::sendEggRespawn(_egg->getID(), _egg->getInitPos());
    }
    if (_player->getHoldingEgg()) {
        auto _egg = _world->getEgg(_player->getEggId());
        if (_egg->getHatched() == false) {
            _egg->setPosition(_player->getPosition());
            _hatchbar->setVisible(true);
            _hatchbar->setProgress(_egg->getDistanceWalked()/80);
            Vec2 diff = _player->getPosition() - _egg->getInitPos();
            float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
            _egg->setDistanceWalked(_egg->getDistanceWalked() + dist);
            _egg->setInitPos(_player->getPosition());
            if (_egg->getDistanceWalked() >= 80) {
                _world->setEggSpawnCooldown(time(NULL));
                _world->addEggSpawn(_egg->getSpawnLoc());
                _player->setHoldingEgg(false);
                _hatchbar->setVisible(false);
                _hatchbar->setProgress(0.0);
                _hatchedTime = time(NULL);
                _egg->setHatched(true);
                _egg->setDistanceWalked(0);
                _world->setCurrEggCount(_world->getCurrEggCount() - 1);
                _player->setElement(_player->getPrevElement());
                _player->incScore(globals::HATCH_SCORE);
                NetworkController::sendEggHatched(_player->getID(), _egg->getID());
            }
        }
    }
    else if (_hatchbar->isVisible()){
        _hatchbar->setVisible(false);
    }
    
    if (time(NULL) - _hatchedTime >= _hatchTextTimer) {
        _hatchnode->setVisible(false);
    }
    
    //cooldown for player after it's tagged
    //for(auto p : _world->getPlayers()){
    //    if (p->getIsTagged()) {
    //        if (time(NULL) - p->getTagCooldown() >= 7) { //tag cooldown is 7 secs rn
    //            p->setIsTagged(false);
    //        }
    //    }
    //}
    
    // ability stuff here
    _abilitybar->setProgress(_player->getOrbScore() * 0.2);
    if (_player->getOrbScore() == 5 && _abilityController.getQueuedAbility() == AbilityController::Ability::NoAbility) {
        _abilityController.updateAbility(_abilityname);
        _abilityname->setVisible(true);
    }
    if (_playerController.isAbilityPressed()) {
        _abilityController.activateAbility(_player);
    }
    _abilityController.deactivateAbility(_player, _abilityname);

  

    _scoreHUD->setText(updateScoreText(_player->getScore()));
    _timerHUD->setText(updateTimerText(_startTime + globals::GAME_TIMER - time(NULL)));
    _framesHUD->setText(updateFramesText(_player->getLinearVelocity().length()));
    
    for(auto p : _world->getPlayers()){
        p->animateMovement();
    }
    
    
    //send new position
    NetworkController::sendPosition();
}



void GameScene::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                            const std::shared_ptr<cugl::scene2::SceneNode>& node,
                            int zOrder,
                            bool useObjPosition) {
    _world->getPhysicsWorld()->addObstacle(obj);
//    obj->setDebugScene(_debugnode);
    obj->setDebugScene(_debugnode);

    // Position the scene graph node (enough for static objects)
      if (useObjPosition) {
          node->setPosition(obj->getPosition()*_scale);
      }
    _world->getSceneNode()->addChild(node, zOrder);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_scale);
            weak->setAngle(obs->getAngle());
        });
    }

}


void GameScene::setMovementStyle(int m) {
    _playerController.setMoveStyle(static_cast<Movement>(m));
}


/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize(float w, float h) const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width/dimen.height;
    float ratio2 = ((float)w)/((float)h);
    if (ratio1 < ratio2) {
        dimen *= w/dimen.width;
    } else {
        dimen *= h/dimen.height;
    }
    return dimen;
}

std::string GameScene::updateScoreText(const int score) {
    stringstream ss;
    ss << "Score: " << score;
    return ss.str();
}

std::string GameScene::updateFramesText(const double score) {
    stringstream ss;
    ss << "Frames: " << score;
    return ss.str();
}

std::string GameScene::updateTimerText(const time_t time) {
    stringstream ss;
    time_t sec = time % 60;
    if (sec < 10) {
        ss << "Timer: " << (time / 60) << ":0" << sec;
    }
    else {
        ss << "Timer: " << (time / 60) << ":" << sec;
    }
    return ss.str();
}

std::map<std::string, int> GameScene::getResults() {
    std::map<std::string, int> results;
    auto players = _world->getPlayers();
    for (int i = 0; i < players.size(); i++) {
        auto p = players[i];
        stringstream ss;
        ss << "player" << " " << p->getID();
        results[ss.str()] = p->getScore();
    }
    
    return results;
}

std::string GameScene::getWinner() {
    stringstream ss;
    auto players = _world->getPlayers();
    int winScore = -1;
    for (int i = 0; i < players.size(); i++) {
        auto p = players[i];
        if (p->getScore() > winScore) {
            winScore = p->getScore();
        }
    }
    
    if (_world->getPlayer(NetworkController::getPlayerId().value())->getScore() == winScore) {
        ss << "Congratulations!";
    }
    else {
        ss << "";
    }
    
    return ss.str();
}
