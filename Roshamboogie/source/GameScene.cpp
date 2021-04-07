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


#define WALL_VERTS  8
#define WALL_COUNT  4
// Left/Right wall padding
#define LR_PADDING  0.5f
// Top/Bottom wall padding
#define TB_PADDING  1.0f

float WALL[WALL_COUNT][WALL_VERTS] = {
    {0.0f, 0.0f,
        LR_PADDING, 0.0f,
        LR_PADDING, DEFAULT_HEIGHT,
        0.0f,  DEFAULT_HEIGHT},
    {LR_PADDING, 0.0f,
        DEFAULT_WIDTH-LR_PADDING,  0.0f,
        DEFAULT_WIDTH-LR_PADDING,  TB_PADDING,
        LR_PADDING, TB_PADDING},
    {DEFAULT_WIDTH-LR_PADDING, 0.0f,
        DEFAULT_WIDTH,  0.0f,
        DEFAULT_WIDTH,  DEFAULT_HEIGHT,
        DEFAULT_WIDTH-LR_PADDING, DEFAULT_HEIGHT},
    {LR_PADDING, DEFAULT_HEIGHT-TB_PADDING,
        DEFAULT_WIDTH-LR_PADDING, DEFAULT_HEIGHT-TB_PADDING,
        DEFAULT_WIDTH-LR_PADDING,  DEFAULT_HEIGHT,
        LR_PADDING, DEFAULT_HEIGHT}
};

#define BASIC_DENSITY   0.0f
/** The density for a bullet */
#define HEAVY_DENSITY   10.0f
/** Friction of most platforms */
#define BASIC_FRICTION  0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION   0.1f

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
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = computeActiveSize();
    Rect rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT);
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    _scale = dimen.width == SCENE_WIDTH ? dimen.width/rect.size.width : dimen.height/rect.size.height;
    
    //create world
    world = World::alloc(assets, DEFAULT_WIDTH, DEFAULT_HEIGHT, _scale, NetworkController::getNumPlayers());
    NetworkController::setWorld(world);
    
    // Start up the input handler
    _assets = assets;
    _playerController.init();

    // Acquire the scene built by the asset loader and resize it the scene
    auto scene_background = _assets->get<scene2::SceneNode>("background");
    scene_background->setContentSize(dimen);
    scene_background->doLayout(); // Repositions the HUD;
    
    auto scene_ui = _assets->get<scene2::SceneNode>("ui");
    scene_ui->setContentSize(dimen);
    scene_ui->doLayout(); // Repositions the HUD;

    _scoreHUD  = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("ui_hud"));
    
    _hatchbar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("ui_bar"));
//    CULog("Hatchbar: %s", _hatchbar->get);
    _hatchbar->setVisible(false);
    
    _hatchnode = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("ui_hatched"));
    _hatchnode->setVisible(false);

    _abilityname = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("ui_abilityName"));
    _abilityname->setVisible(false);

    _abilitybar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("ui_abilityBar"));
    _abilitybar->setForegroundColor(Color4(255, 255, 0));
    
    _roomIdHUD = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("ui_roomId"));
    
    auto _world = world->getPhysicsWorld();
    _world->activateCollisionCallbacks(true);
    if(NetworkController::isHost()){
        _world->onBeginContact = [this](b2Contact* contact) {
            CollisionController::beginContact(contact);
        };
        _world->onEndContact = [this](b2Contact* contact) {
            CollisionController::endContact(contact);
        };
    }
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        CollisionController::beforeSolve(contact,oldManifold);
    };
    
    Vec2 offset((dimen.width-SCENE_WIDTH)/2.0f,(dimen.height-SCENE_HEIGHT)/2.0f);

    auto _worldnode = world->getSceneNode();
    _worldnode->setPosition(offset);
    
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
    
    world->setDebugNode(_debugnode);
    
    addChild(scene_background);
    addChild(_worldnode);
    addChild(_debugnode);
    addChild(scene_ui);
    reset();
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _debugnode = nullptr;
        _scoreHUD = nullptr;
        _hatchnode = nullptr;
        _hatchbar = nullptr;
        _abilitybar = nullptr;
        _abilityname = nullptr;
        _active = false;
        _debug = false;
        Scene2::dispose();
        world = nullptr;
    }
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    _debugnode->removeAllChildren();
    world->reset();
    
    auto idopt = NetworkController::getPlayerId();
    if(idopt.has_value()){
        auto _player = world->getPlayer(idopt.value());
        _player->setUsername(NetworkController::getUsername());
        getCamera()->translate(_player->getSceneNode()->getPosition() - getCamera()->getPosition());
    }
    _playerController.init();
        
    populate();
    
    setDebug(false);

    getCamera()->update();
}

void GameScene::update(float timestep) {
    
    if (_playerController.didDebug()) { setDebug(!isDebug()); }
    
    // NETWORK //
    
    /*std::string currRoomId = NetworkController::getRoomId();
    _roomIdHUD->setText(currRoomId);*/
//    NetworkController::step();
    NetworkController::update(timestep);
    if (_currRoomId == "") {
        _currRoomId = NetworkController::getRoomId();
        stringstream ss;
        ss << "Room Id: " << _currRoomId;
        _roomIdHUD->setText(ss.str());
    }
    
    if (_playerController.didDebug()) { setDebug(!isDebug()); }
    
    
    // BEGIN PLAYER MOVEMENT //
    
    auto playerId_option = NetworkController::getPlayerId();
    if(! playerId_option.has_value()) return;
    uint8_t playerId = playerId_option.value();
    auto _player = world->getPlayer(playerId);
    
    _playerController.readInput();
    switch (_playerController.getMoveStyle()) {
        case Movement::AlwaysForward: {
            if (_abilityController.getActiveAbility() == AbilityController::Ability::SpeedBoost) {
                break;
            }
            auto ang = _player->getAngle() + _playerController.getMov().x * M_PI / -30.0f;
            _player->setAngle(ang > M_PI ? ang - 2.0f*M_PI : (ang < -M_PI ? ang + 2.0f*M_PI : ang));
            
            auto vel = _player->getLinearVelocity();
            auto offset = vel.getAngle() - _player->getAngle() + M_PI / 2.0f;
            offset = offset > M_PI ? offset - 2.0f * M_PI : (offset < -M_PI ? offset + 2.0f * M_PI : offset);
            auto correction = _player->getLinearVelocity().rotate(-1.0f * offset - M_PI / 2.0f).scale(sin(offset) * .02f);
            _player->setLinearVelocity(vel.add(correction));
            if (_playerController.getMov().x == 0) {
                //if (offset < M_PI / 2.0f && offset > -M_PI / 2.0f) {
                _player->applyForce();
                //}
            }
            else {
                auto forForce = _player->getForce();
                auto turnForce = _player->getForce().getPerp().scale(vel.length() * cos(offset) * -1.1f);
                if (_playerController.getMov().x > 0) {
                    turnForce.scale(-1.0f);
                }
                if (offset < M_PI / 2.0f && offset > -M_PI / 2.0f) {
                    turnForce.scale(-1.0f);
                    _player->applyForce();
                }
                _player->setForce(turnForce);
                _player->applyForce();
                _player->setForce(forForce);
            }
            break;
        }
        case Movement::SwipeForce:{
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
        default:
            break;
    }
    
        world->getPhysicsWorld()->update(timestep);

    auto after = _player->getSceneNode()->getPosition();
    auto camSpot = getCamera()->getPosition();
    auto trans = after - camSpot;
    getCamera()->translate(trans*.05f);
    getCamera()->update();


//    if(NetworkController::isHost()){
//        if (orbShouldMove) {
//            std::random_device r;
//            std::default_random_engine e1(r());
//            std::uniform_int_distribution<int> rand_int(1, 31);
//            std::uniform_int_distribution<int> rand_int2(1, 17);
//            _orbTest->setPosition(rand_int(e1), rand_int2(e1));
//        }
//
//        orbShouldMove = false;
//    }
    
    if(NetworkController::isHost()){
        for(int i = 0; i < 3; ++i){ //TODO: This is temporary;
            auto orb = world->getOrb(i);
            if(orb->getCollected()) {
                orb->respawn();
                NetworkController::sendOrbRespawn(orb->getID(), orb->getPosition());
                _player->setOrbScore( _player->getOrbScore() + 1);
            }
            orb->setCollected(false);

        }
    }
    
    
    //egg hatch logic
    auto _egg = world->getEgg(0);
    if (_egg->getCollected() && _egg->getHatched() == false) {
        std::shared_ptr<Player> _eggCollector = world->getPlayer(_egg->getPID());
        _egg->setPosition(_eggCollector->getPosition());
        if (_egg->getPID() == _player->getID()) {
            _hatchbar->setVisible(true);
        }
        _hatchbar->setProgress(_egg->getDistanceWalked()/80);
        Vec2 diff = _eggCollector->getPosition() - _egg->getInitPos();
        float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
        _egg->incDistanceWalked(dist);
        _egg->setInitPos(_eggCollector->getPosition());
        if (_egg->getDistanceWalked() >= 80) {
            _hatchbar->dispose();
            _hatchedTime = clock();
            _egg->setHatched(true);
            _egg->dispose();
//            _egg->setCollected(false);
            _score += 10;
            _eggCollector->setElement(_eggCollector->getPrevElement());
            if (_egg->getPID() == _player->getID()) {
                _hatchnode->setVisible(true);
            }
            CULog("hatched");
        }
        
    }
    
    if (clock() - _hatchedTime >= _hatchTextTimer) {
        _hatchnode->setVisible(false);
    }
    
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

    // player tagging
//    if (_player->getDidTag()) {
//        _score += 15;
//    }
//    if (_player->getIsTagged()) {
//        _player->setPosition(Vec2(20,10));
//    }
//    if (_player2->getIsTagged()) {
//        _player2->setPosition(Vec2(20,10));
//    }

    _scoreHUD->setText(updateScoreText(_score));
    
    //send new position
    NetworkController::sendPosition();
}

void GameScene::populate() {
    std::shared_ptr<Texture> image = _assets->get<Texture>("earth");
    std::shared_ptr<scene2::PolygonNode> sprite;
    std::shared_ptr<scene2::WireNode> draw;
    std::string wname = "wall";
    for (int ii = 0; ii<WALL_COUNT; ii++) {
        std::shared_ptr<physics2::PolygonObstacle> wallobj;

        Poly2 wall(WALL[ii],WALL_VERTS);
        // Call this on a polygon to get a solid shape
        SimpleTriangulator triangulator;
        triangulator.set(wall);
        triangulator.calculate();
        wall.setIndices(triangulator.getTriangulation());
        wall.setGeometry(Geometry::SOLID);

        wallobj = physics2::PolygonObstacle::alloc(wall);
        wallobj->setDebugColor(Color4::WHITE);
        // You cannot add constant "".  Must stringify
        wallobj->setName(std::string("wall")+cugl::strtool::to_string(ii));
        wallobj->setName(wname);

        // Set the physics attributes
        wallobj->setBodyType(b2_staticBody);
        wallobj->setDensity(BASIC_DENSITY);
        wallobj->setFriction(BASIC_FRICTION);
        wallobj->setRestitution(BASIC_RESTITUTION);

        wall *= _scale;
        sprite = scene2::PolygonNode::allocWithTexture(image,wall);
        addObstacle(wallobj,sprite,2);
    }

}


void GameScene::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                            const std::shared_ptr<cugl::scene2::SceneNode>& node,
                            int zOrder,
                            bool useObjPosition) {
    world->getPhysicsWorld()->addObstacle(obj);
//    obj->setDebugScene(_debugnode);
    obj->setDebugScene(_debugnode);

    // Position the scene graph node (enough for static objects)
      if (useObjPosition) {
          node->setPosition(obj->getPosition()*_scale);
      }
      world->getSceneNode()->addChild(node, zOrder);

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
Size GameScene::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width/dimen.height;
    float ratio2 = ((float)SCENE_WIDTH)/((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH/dimen.width;
    } else {
        dimen *= SCENE_HEIGHT/dimen.height;
    }
    return dimen;
}

std::string GameScene::updateScoreText(const int score) {
    stringstream ss;
    ss << "Score: " << score;
    return ss.str();
}
