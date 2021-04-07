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
#define DEFAULT_WIDTH   150.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  75.0f


#define WALL_VERTS  8
#define WALL_COUNT  16
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
        LR_PADDING, DEFAULT_HEIGHT},
    {20.0f, DEFAULT_HEIGHT,
        21.0f, DEFAULT_HEIGHT,
        21.0f, DEFAULT_HEIGHT-30.0,
        20.0f, DEFAULT_HEIGHT-30.0},
    {20.0f, 25.0f,
        70.0f, 25.0f,
        70.0f, 26.0f,
        20.0f, 26.0f},
    {
        45.0f,25.0f,
        46.0f,25.0f,
        46.0f,60.0f,
        45.0f,60.0f
    },
    {
        60.0f,60.0f,
        60.0f,59.0f,
        85.0f,59.0f,
        85.0f,60.0f
    },
    {
        85.0f,60.0f,
        84.0f,60.0f,
        84.0f,20.0f,
        85.0f,20.0f
    },
    {
        85.0f,20.0f,
        85.0f,21.0f,
        95.0f,21.0f,
        95.0f,20.0f
    },
    {
        85.0f,46.0f,
        85.0f,47.0f,
        105.0f,47.0f,
        105.0f,46.0f
    },
    {
        105.0f,60.0f,
        106.0f,60.0f,
        106.0f,75.0f,
        105.0f,75.0f
    },
    {
        105.0f,33.0f,
        105.0f,34.0f,
        130.0f,34.0f,
        130.0f,33.0f
    },
    {
        130.0f,60.0f,
        131.0f,60.0f,
        131.0f,20.0f,
        130.0f,20.0f
    },
    {
        120.0f,20.0f,
        140.0f,20.0f,
        140.0f,21.0f,
        120.0f,21.0f
    },
    {
        140.0f,35.0f,
        140.0f,36.0f,
        150.0f,36.0f,
        150.0f,35.0f
    }
    
};

#define BASIC_DENSITY   0.0f
/** The density for a bullet */
#define HEAVY_DENSITY   10.0f
/** Friction of most platforms */
#define BASIC_FRICTION  0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION   0.1f
/** The restitution for all physics objects */
#define TURNS_PER_SPIN   55.0f
/** how much the lateral velocity is subtracted per frame*/
#define KINETIC_FRICTION 1.4f

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
    SpawnController::setWorld(world);
    CollisionController::setWorld(world);
    
    // Start up the input handler
    _assets = assets;
    _playerController.init();
    // Acquire the scene built by the asset loader and resize it the scene
    auto scene_background = _assets->get<scene2::SceneNode>("background");
    scene_background->setContentSize(dimen);
    scene_background->doLayout(); // Repositions the HUD;
    
    _UInode = _assets->get<scene2::SceneNode>("ui");
    _UInode->setContentSize(dimen);
    _UInode->doLayout(); // Repositions the HUD;

    _scoreHUD  = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("ui_hud"));
    
    _hatchbar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("ui_bar"));
//    CULog("Hatchbar: %s", _hatchbar->get);
    _hatchbar->setVisible(false);
    
    _hatchnode = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("ui_hatched"));
    _hatchnode->setVisible(false);
    
//    _roomIdHUD = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("ui_roomId"));
    
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
    addChild(_UInode);
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
        _player->setIsLocal(true);
        getCamera()->translate(_player->getSceneNode()->getPosition() - getCamera()->getPosition());
//        getCamera()->setZoom(2.0f);
    }
    _playerController.init();
    
//    if (NetworkController::isHost()) {
//        SpawnController::initSpawn();
//    }

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
    auto _player = world->getPlayer(playerId);
    
    _playerController.readInput();
    switch (_playerController.getMoveStyle()) {
        case Movement::AlwaysForward: {
            auto ang = _player->getAngle() + _playerController.getMov().x * -2.0f * M_PI / TURNS_PER_SPIN;
            _player->setAngle(ang > M_PI ? ang - 2.0f*M_PI : (ang < -M_PI ? ang + 2.0f*M_PI : ang));
            
            auto vel = _player->getLinearVelocity();
            //Please don't delete this comment, angles were difficult to derive and easy to forget
            //vel angle originates from x axis, player angle orginates from y axis
            auto offset = vel.getAngle() - _player->getAngle() + M_PI / 2.0f;
            offset = offset > M_PI ? offset - 2.0f * M_PI : (offset < -M_PI ? offset + 2.0f * M_PI : offset);

            auto correction = _player->getLinearVelocity().rotate(-1.0f * offset - M_PI / 2.0f).scale(sin(offset));
            if (correction.length() > KINETIC_FRICTION) {
                correction.scale( KINETIC_FRICTION / correction.length());
            }
            _player->setLinearVelocity(vel.add(correction));

            if (_playerController.getMov().x == 0) {
                //constant acceleration
                //_player->applyForce();

                auto big = _player->getMass();

                //accelerate to a maximum velocity
                auto forForce = _player->getForce();
                auto scaling = _player->getForce();
                //scaling.normalize().scale(0.05f * pow(30.0f - vel.length(), 2.0f));
                scaling.normalize().scale(_player->getMass() * 0.32f * (140.0f - vel.length()));
                //scaling.normalize().scale(2.0f * pow(30.0f - vel.length(), 0.6f));
                _player->setForce(scaling);
                _player->applyForce();
                _player->setForce(forForce);
            }
            else {
                auto forForce = _player->getForce();
                auto turnForce = _player->getForce().getPerp().scale(vel.length() * cos(offset) * 4.0f * _player->getMass() * tan(M_PI / TURNS_PER_SPIN));
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
    
    world->getPhysicsWorld()->update(timestep);

    auto playPos = _player->getSceneNode()->getPosition();
    auto camSpot = getCamera()->getPosition();
    auto trans = (playPos - camSpot)*.07f;
    getCamera()->translate(trans);
    getCamera()->update();
    _UInode->setPosition(camSpot + trans - Vec2(SCENE_WIDTH/2.0f, SCENE_HEIGHT/2.0f));
    
    
    if(NetworkController::isHost()){
//        for(int i = 0; i < 3; ++i){ //TODO: This is temporary;
//            auto orb = world->getOrb(i);
//            if(orb->getCollected()) {
//                orb->respawn();
//                NetworkController::sendOrbRespawn(orb->getID(), orb->getPosition());
//            }
//            orb->setCollected(false);
//
//        }
        
        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> prob(0,100);
//        CULog("prob %d", prob(e1));
        if (prob(e1) < 25) { //TODO: change to depend on how many orbs on map currently
//            CULog("curr orbs %d", world->getCurrOrbCount());
            if (world->getCurrOrbCount() < 10) {
                SpawnController::spawnOrbs();
            }
        }
    
    }
    
    
    //egg hatch logic
    //TODO: change to allow multiple eggs
    auto _egg = world->getEgg(0);
    if (_egg->getCollected() && _egg->getHatched() == false) {
        std::shared_ptr<Player> _eggCollector = world->getPlayer(_egg->getPID());
        _egg->setPosition(_eggCollector->getPosition());
        if (_egg->getPID() == _player->getID()) {
            _hatchbar->setVisible(true);
        }
        else {
            _hatchbar->setVisible(false);
        }
        _hatchbar->setProgress(_egg->getDistanceWalked()/80);
        Vec2 diff = _eggCollector->getPosition() - _egg->getInitPos();
        float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
        _egg->incDistanceWalked(dist);
        _egg->setInitPos(_eggCollector->getPosition());
        if (_egg->getDistanceWalked() >= 80) {
            _hatchbar->dispose();
            _hatchedTime = time(NULL);
            _egg->setHatched(true);
            _egg->dispose();
//            _egg->setCollected(false);
            _eggCollector->setElement(_eggCollector->getPrevElement());
            if (_egg->getPID() == _player->getID()) {
                _hatchnode->setVisible(true);
                _player->incScore(10);
            }
//            CULog("hatched");
        }
        
    }
    
    if (time(NULL) - _hatchedTime >= _hatchTextTimer) {
        _hatchnode->setVisible(false);
    }
    
    //cooldown for player after it's tagged
    for(auto p : world->getPlayers()){
        if (p->getIsTagged()) {
            if (time(NULL) - p->getTagCooldown() >= 7) { //tag cooldown is 7 secs rn
                CULog("not tagged");
    //            _player->getSceneNode()->setVisible(false);
                p->setIsTagged(false);
            }
        }
    }
    

    _scoreHUD->setText(updateScoreText(_player->getScore()));
    
    //send new position
    //TODO: only every few frames
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
