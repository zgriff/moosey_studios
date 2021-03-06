//
//  RPSGameScene.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "RPSGameScene.h"
#include "RPSCollisionController.h"

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

/** Number of rows in the ship image filmstrip */
#define SHIP_ROWS   4
/** Number of columns in this ship image filmstrip */
#define SHIP_COLS   5
/** Number of elements in this ship image filmstrip */
#define SHIP_SIZE   18

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
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("lab");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD;

    _blueSound = _assets->get<Sound>("laser");
    _redSound = _assets->get<Sound>("fusion");
    addChild(scene);
    reset();
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    auto root = getChild(0);
    if (_blueShip != nullptr && _blueShip->getSceneNode() != nullptr) {
        root->removeChild(_blueShip->getSceneNode());
    }
    if (_redShip != nullptr && _redShip->getSceneNode() != nullptr) {
        root->removeChild(_redShip->getSceneNode());
    }
    Size dimen = root->getContentSize();
    
    auto shipTexture = _assets->get<Texture>("ship");

    _blueShip = Ship::alloc(dimen.width*(2.0f / 3.0f), dimen.height*(1.0f / 2.0f), 90);
    _blueShip->setColor(Color4f(0.5f, 0.5f, 1.0f, 1.0f));   // Blue, but makes texture easier to see
    _blueShip->setTextures(shipTexture, dimen.width, dimen.height);
    _blueShip->setSID(0);
    _blueController.init(0);

    _redShip = Ship::alloc(dimen.width*(1.0f / 3.0f), dimen.height*(1.0f / 2.0f), -90);
    _redShip->setColor(Color4f(1.0f, 0.25f, 0.25f, 1.0f));  // Red, but makes texture easier to see
    _redShip->setTextures(shipTexture, dimen.width, dimen.height);
    _redShip->setSID(1);
    _redController.init(1);
    
    root->addChild(_redShip->getSceneNode());
    root->addChild(_blueShip->getSceneNode());
}

void GameScene::update(float timestep) {
    // Read the keyboard for each controller.
    _redController.readInput();
    _blueController.readInput();
    
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
    _redShip->move( _redController.getForward(),  _redController.getTurn());
    _blueShip->move(_blueController.getForward(), _blueController.getTurn());
    
    collisions::checkForCollision(_blueShip, _redShip);
    collisions::checkInBounds(_blueShip, getBounds());
    collisions::checkInBounds(_redShip, getBounds());
}

/**
 * Draws all this scene to the given SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch.  By overriding it, you can do custom drawing
 * in its place.
 *
 * @param batch     The SpriteBatch to draw with.
 */
void GameScene::render(const std::shared_ptr<cugl::SpriteBatch>& batch) {
    // Call SUPER to do standard rendering
    Scene2::render(batch);
    
    // Now do 3152-style rendering for the photons
    batch->begin(getCamera()->getCombined());
    batch->setBlendFunc(GL_ONE, GL_ONE); // Additive blending
    batch->end();

}
