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
#define DEFAULT_WIDTH   36.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f

/** The initial rocket position */
float PLAYER_POS[] = {24,  4};

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
    
    _roomIdHUD = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("ui_roomId"));
    
    _world = physics2::ObstacleWorld::alloc(rect,Vec2::ZERO);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
        CollisionController::beginContact(contact);
    };
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        CollisionController::beforeSolve(contact,oldManifold);
    };
    _scale = dimen.width == SCENE_WIDTH ? dimen.width/rect.size.width : dimen.height/rect.size.height;
    Vec2 offset((dimen.width-SCENE_WIDTH)/2.0f,(dimen.height-SCENE_HEIGHT)/2.0f);

    // Create the scene graph
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    addChild(scene_background);
    addChild(_worldnode);
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
        _world = nullptr;
        _worldnode = nullptr;
        _scoreHUD = nullptr;
        _hatchnode = nullptr;
        _hatchbar = nullptr;
        _active = false;
        Scene2::dispose();
    }
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    _world->clear();
    _worldnode->removeAllChildren();
//    auto root = getChild(0);
    
    auto shipTexture = _assets->get<Texture>("rocket");
    auto orbTexture = _assets->get<Texture>("photon");
    auto swapStTexture = _assets->get<Texture>("swapstation");
    auto eggTexture = _assets->get<Texture>("target");

    Vec2 playerPos = ((Vec2)PLAYER_POS);
    Size playerSize(shipTexture->getSize()/_scale);
    _player = Player::alloc(playerPos, playerSize, Element::Water);
    _player2 = Player::alloc(playerPos, playerSize, Element::Water);
    _world->addObstacle(_player);
    _player->setTextures(shipTexture);
    _player->setID(0);
    _player->setDrawScale(_scale);
    _playerController.init();
    _world->addObstacle(_player2);
    _player2->setTextures(shipTexture);
    _player2->setID(1);
    _player2->setDrawScale(_scale);
    
    //creating the three orbs
    _fireOrb = Orb::alloc(Vec2(4,4), Element::Fire);
    _world->addObstacle(_fireOrb);
    _fireOrb->setTextures(orbTexture);
    _fireOrb->setDrawScale(_scale);
    
    _waterOrb = Orb::alloc(Vec2(20,8), Element::Water);
    _world->addObstacle(_waterOrb);
    _waterOrb->setTextures(orbTexture);
    _waterOrb->setDrawScale(_scale);
    
    _grassOrb = Orb::alloc(Vec2(10,12), Element::Grass);
    _world->addObstacle(_grassOrb);
    _grassOrb->setTextures(orbTexture);
    _grassOrb->setDrawScale(_scale);
    
    
    Vec2 swapStPos = Vec2(8,8);
    Size swapStSize(swapStTexture->getSize() / _scale);
    _swapStation = SwapStation::alloc(swapStPos, swapStSize);
    _world->addObstacle(_swapStation);
    _swapStation->setTextures(swapStTexture);
    _swapStation->setDrawScale(_scale);
    _swapStation->setActive(true);
    
    Vec2 eggPos = Vec2(14,14);
    Size eggSize(eggTexture->getSize() / _scale);
    _egg = Egg::alloc(eggPos, eggSize);
    _world->addObstacle(_egg);
    _egg->setTextures(eggTexture);
    _egg->setDrawScale(_scale);
    _egg->setActive(true);
    
    populate();
    
    _worldnode->addChild(_fireOrb->getSceneNode());
    _worldnode->addChild(_waterOrb->getSceneNode());
    _worldnode->addChild(_grassOrb->getSceneNode());
    _worldnode->addChild(_player->getSceneNode());
    _worldnode->addChild(_player2->getSceneNode());
    _worldnode->addChild(_swapStation->getSceneNode());
    _worldnode->addChild(_egg->getSceneNode());
}

void GameScene::update(float timestep) {
    /*std::string currRoomId = NetworkController::getRoomId();
    _roomIdHUD->setText(currRoomId);*/
//    NetworkController::step();
    NetworkController::receive([&](const std::vector<uint8_t> msg) {
        ND::NetworkData nd;
        ND::fromBytes(nd, msg);
        if(nd.packetType == ND::NetworkData::CLIENT_PACKET){
            _player2->setPosition(nd.clientData.playerPos_x, nd.clientData.playerPos_y);
            _player2->setLinearVelocity(nd.clientData.playerVel_x, nd.clientData.playerVel_y);
        }
    });
    if (_currRoomId == "") {
        _currRoomId = NetworkController::getRoomId();
        stringstream ss;
        ss << "Room Id: " << _currRoomId;
        _roomIdHUD->setText(ss.str());
    }
    _playerController.readInput();
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
    

    _world->update(timestep);
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

    
    if (_fireOrb->getCollected()) {
        _fireOrb->respawn();
        _score += 1;
    }
    
    if (_waterOrb->getCollected()) {
        _waterOrb->respawn();
        _score += 1;
    }
    
    if (_grassOrb->getCollected()) {
        _grassOrb->respawn();
        _score += 1;
    }
    
    //egg hatch logic
    if (_egg->getCollected() && _egg->getHatched() == false) {
        _egg->setPosition(_player->getPosition());
        _hatchbar->setVisible(true);
        _hatchbar->setProgress(_egg->getDistanceWalked()/80);
        Vec2 diff = _player->getPosition() - _egg->getInitPos();
        float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
        _egg->incDistanceWalked(dist);
        _egg->setInitPos(_player->getPosition());
        if (_egg->getDistanceWalked() >= 80) {
            _hatchbar->dispose();
            _hatchedTime = clock();
            _egg->setHatched(true);
            _egg->dispose();
//            _egg->setCollected(false);
            _score += 10;
            _player->setElement(_player->getPrevElement());
            _hatchnode->setVisible(true);
            CULog("hatched");
        }
    }
    
    if (clock() - _hatchedTime >= _hatchTextTimer) {
        _hatchnode->setVisible(false);
    }
    
    _fireOrb->setCollected(false);
    _waterOrb->setCollected(false);
    _grassOrb->setCollected(false);
    _scoreHUD->setText(updateScoreText(_score));
    
    //send new position
    ND::NetworkData nd;
    nd.packetType = ND::NetworkData::PacketType::CLIENT_PACKET;
    nd.clientData.playerPos_x = _player->getPosition().x;
    nd.clientData.playerPos_y = _player->getPosition().y;
    nd.clientData.playerVel_x = _player->getLinearVelocity().x;
    nd.clientData.playerVel_y = _player->getLinearVelocity().y;
    std::vector<uint8_t> bytes;
    ND::toBytes(bytes, nd);
//    for(int i=0; i < bytes.size(); i++){
//        int tmp = bytes.at(i);
//        std::cout << tmp << ' ';
//    }
//    std::cout << endl << "-----------------------" << endl;
       
    NetworkController::send(bytes);
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
    _world->addObstacle(obj);
//    obj->setDebugScene(_debugnode);

    // Position the scene graph node (enough for static objects)
      if (useObjPosition) {
          node->setPosition(obj->getPosition()*_scale);
      }
      _worldnode->addChild(node, zOrder);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_scale);
            weak->setAngle(obs->getAngle());
        });
    }
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
