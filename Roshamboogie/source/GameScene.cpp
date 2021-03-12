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

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 720

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f

/** The initial rocket position */
float PLAYER_POS[] = {24,  4};

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
    _playerController.init(getBounds());
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("lab");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD;

    
    _world = physics2::ObstacleWorld::alloc(rect,Vec2::ZERO);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
        beginContact(contact);
    };
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        beforeSolve(contact,oldManifold);
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

    Vec2 playerPos = ((Vec2)PLAYER_POS);
    Size playerSize(shipTexture->getSize()/_scale);
    _player = Player::alloc(playerPos, playerSize);
    _world->addObstacle(_player);
    _player->setTextures(shipTexture);
    _player->setID(0);
    _player->setDrawScale(_scale);
    _playerController.init(getBounds());
    
    _orbTest = Orb::alloc(Element::Fire);
    _world->addObstacle(_orbTest);
    _orbTest->setTextures(orbTexture);
    _orbTest->setDrawScale(_scale);
    
    
    _worldnode->addChild(_orbTest->getSceneNode());
    _worldnode->addChild(_player->getSceneNode());
}

void GameScene::update(float timestep) {
    // Read the keyboard for each controller.

//    _playerController.readInput();
//    _player->setMovement(_playerController.getSwing());
    _playerController.update(timestep);
    _player->setForce(_playerController.getSwing()*_player->getThrust());
    _player->setMovement(_playerController.getSwing());
    _player->applyForce();
    if  (_player->getForce().isNearZero(5.0f)) {
        _playerController.setSwingFinish(true);
    }
    _world->update(timestep);
    if(orbShouldMove){
        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> rand_int(0, 32);
        std::uniform_int_distribution<int> rand_int2(0, 18);
        _orbTest->setPosition(rand_int(e1), rand_int2(e1));
    }
    
    orbShouldMove = false;

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

void GameScene::beginContact(b2Contact* contact){
    b2Fixture * fixA = contact->GetFixtureA();
    b2Body * bodyA = fixA->GetBody();
    b2Fixture * fixB = contact->GetFixtureB();
    b2Body * bodyB = fixB->GetBody();
    
    cugl::physics2::Obstacle* bd1 = (cugl::physics2::Obstacle*) bodyA->GetUserData();
    cugl::physics2::Obstacle* bd2 = (cugl::physics2::Obstacle*) bodyB->GetUserData();
    
    if(bd1->getName() == "orb" && bd2->getName() == "player"){
//        ((Player *) bd2)->
        orbShouldMove = true;
    }else if(bd2->getName() == "orb" && bd1->getName() == "player"){
        orbShouldMove = true;
    }
}

void GameScene::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold){
    
}
