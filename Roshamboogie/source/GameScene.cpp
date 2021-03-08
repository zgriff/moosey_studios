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
//#include "CollisionController.h"

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

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

    Vec2 playerPos = ((Vec2)PLAYER_POS);
    Size playerSize(shipTexture->getSize()/_scale);
    
    _player = Player::alloc(playerPos, playerSize);
    _player->setPosition(playerPos);
    CULog("player pos --  X: %f , Y: %f  ", _player->getPosition().x,_player->getPosition().y);
    _player->setTextures(shipTexture);
    _player->setID(0);
    _player->setDrawScale(_scale);
    _playerController.init(getBounds());
    
    _worldnode->addChild(_player->getSceneNode());
    _world->addObstacle(_player);
}

void GameScene::update(float timestep) {
    // Read the keyboard for each controller.
//    _playerController.readInput();
//    _player->setMovement(_playerController.getSwing());
    _playerController.update(timestep);
    float touchScaleX = DEFAULT_WIDTH/getBounds().getMaxX();
    float touchScaleY = DEFAULT_HEIGHT/getBounds().getMaxY();
    float touchX = _playerController.getPath().x*touchScaleX;
    float touchY = _playerController.getPath().x*touchScaleY;
    Vec2 touch = Vec2(touchX, touchY);
    CULog("touch pos --  X: %f , Y: %f  ", touch.x,touch.y);
    touch = touch - _player->getPosition();
    CULog("touch pos --  X: %f , Y: %f  ", touch.x,touch.y);
    CULog("player pos --  X: %f , Y: %f  ", _player->getPosition().x,_player->getPosition().y);
//    _player->setMovement(touch);
    _player->setForce(touch);
    _player->applyForce();
    _world->update(timestep);
    
//    // Move the photons forward, and add new ones if necessary.
//    if (_redController.didPressFire() && firePhoton(_redShip)) {
//        // The last argument is force=true.  It makes sure only one instance plays.
//        AudioEngine::get()->play("redfire", _redSound, false, 1.0f, true);
//    }
//    if (_blueController.didPressFire() && firePhoton(_blueShip)) {
//        // The last argument is force=true.  It makes sure only one instance plays.
//        AudioEngine::get()->play("bluefire", _blueSound, false, 1.0f, true);
//    }

    // Move the ships and photons forward (ignoring collisions)
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
