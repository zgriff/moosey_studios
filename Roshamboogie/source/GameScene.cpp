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
#include "CollisionController.h"

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <random>

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
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("lab");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD;

    _scoreHUD  = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("lab_hud"));
    
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
    addChild(scene);
    addChild(_worldnode);
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
    _world->addObstacle(_player);
    _player->setTextures(shipTexture);
    _player->setID(0);
    _player->setDrawScale(_scale);
    _playerController.init();
    
    _orbTest = Orb::alloc(Element::Fire);
    _world->addObstacle(_orbTest);
    _orbTest->setTextures(orbTexture);
    _orbTest->setDrawScale(_scale);
    
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
    
    _worldnode->addChild(_orbTest->getSceneNode());
    _worldnode->addChild(_player->getSceneNode());
    _worldnode->addChild(_swapStation->getSceneNode());
    _worldnode->addChild(_egg->getSceneNode());
    
    
    
}

void GameScene::update(float timestep) {
    // Read the keyboard for each controller.
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
    
    if (_orbTest->getCollected()) {
        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> rand_int(1, 31);
        std::uniform_int_distribution<int> rand_int2(1, 17);
        _orbTest->setPosition(rand_int(e1), rand_int2(e1));
        _score += 1;
    }
    
    //egg hatch logic
    if (_egg->getCollected() && _egg->getHatched() == false) {
        Vec2 diff = _player->getPosition() - _egg->getInitPos();
        float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
        _egg->incDistanceWalked(dist);
        _egg->setInitPos(_player->getPosition());
        if (_egg->getDistanceWalked() >= 80) {
            _egg->setHatched(true);
//            _egg->setCollected(false);
            _score += 10;
            _player->setElement(_player->getPrevElement());
            CULog("hatched");
        }
    }
    
    
    _orbTest->setCollected(false);
    _scoreHUD->setText(updateScoreText(_score));
    
    
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
        addObstacle(wallobj,sprite,1);
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
