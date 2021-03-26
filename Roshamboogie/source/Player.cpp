//
//  Player.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "Player.h"
#include "Element.h"
#include "NetworkController.h"

using namespace cugl;

// Default physics values
/** The density of this rocket */
#define DEFAULT_DENSITY 1.0f
/** The friction of this rocket */
#define DEFAULT_FRICTION 0.5f

/** The restitution of this rocket */
#define DEFAULT_RESTITUTION 0.4f
/** The constant force applied to this rocket */
#define DEFAULT_PLAYER_FORCE Vec2(0.0f, 8.3f)
/** Number of rows in the player image filmstrip */
#define PLAYER_ROWS       3
/** Number of columns in this player image filmstrip */
#define PLAYER_COLS       1
/** Number of elements in this player image filmstrip */
#define PLAYER_FRAMES     3

/**
 * Sets the textures for this player.
 *
 * @param player      The texture for the player filmstrip
 */
void Player::setTextures(const std::shared_ptr<Texture>& player) {

    _sceneNode = scene2::PolygonNode::alloc();
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _animationNode = scene2::AnimationNode::alloc(player, PLAYER_ROWS, PLAYER_COLS, PLAYER_FRAMES);
    _animationNode->setAnchor(Vec2::ANCHOR_CENTER);
//    _animationNode->setFrame(0);
    _animationNode->setPosition(0,0);
    _sceneNode->addChild(_animationNode);
    
    _texture = player;
    setElement(_currElt);
    _body->SetUserData(this);

}


void Player::setElement(Element e){
    _prevElt = _currElt;
    _currElt = e;
    
    switch(e){ 
        case Element::Grass:
            _animationNode->setFrame(2);
            _sceneNode->setColor(Color4(255, 255, 255));
            break;
        case Element::Fire:
            _animationNode->setFrame(0);
            _sceneNode->setColor(Color4(255, 255, 255));
            break;
        case Element::Water:
            _animationNode->setFrame(1);
            _sceneNode->setColor(Color4(255, 255, 255));
            break;
        case Element::None:
            _sceneNode->setColor(Color4(0, 0, 0));
            break;
    }
    
}

Element Player::getPreyElement() {
    switch(_currElt){
        case Element::Grass:
            return Element::Water;
        case Element::Fire:
            return Element::Grass;
        case Element::Water:
            return Element::Fire;
        case Element::None:
            return Element::None;
    }
}

void Player::allocUsernameNode(const std::shared_ptr<cugl::Font>& font) {
    _usernameNode = scene2::Label::alloc(_username, font);
    _usernameNode->setPosition(-1*_usernameNode->getContentWidth()/2, 40);
    /*Hardcoded height because not sure how to get dimensions of the Player
    CULog("this width %d", this->getWidth());
    CULog("sceneNode width %d", _sceneNode->getContentWidth());
    CULog("animationNode width %d", _animationNode->getContentWidth());*/
    _sceneNode->addChild(_usernameNode);
}

/**
 * Disposes the player, releasing all resources.
 */
void Player::dispose() {
    // Garbage collect
    _sceneNode = nullptr;
    _animationNode = nullptr;
    _texture = nullptr;
}

/**
 * Initializes a new player at the given location with the given facing.
 *
 * This method does NOT create a scene graph node for this player.  You
 * must call setTextures for that.
 *
 * @param x The initial x-coordinate of the center
 * @param y The initial y-coordinate of the center
 * @param ang The initial angle of rotation
 *
 * @return true if the initialization was successful
 */
bool Player::init(const cugl::Vec2 pos, const cugl::Size size, Element elt) {
    if(physics2::BoxObstacle::init(pos,size)){
        std::string name("player");
        setName(name);
        setDensity(DEFAULT_DENSITY);
        setFriction(DEFAULT_FRICTION);
        setRestitution(DEFAULT_RESTITUTION);
        setFixedRotation(true);
        setForce(DEFAULT_PLAYER_FORCE);
        _currElt = elt;
        _prevElt = elt;
        _isTagged = false;
        _didTag = false;
        _sceneNode = nullptr;
        return true;
    }
    return false;
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * This method is called AFTER the collision resolution state. Therefore, it
 * should not be used to process actions or any other gameplay information.
 * Its primary purpose is to adjust changes to the fixture, which have to
 * take place after collision.
 *
 * In other words, this is the method that updates the scene graph.  If you
 * forget to call it, it will not draw your changes.
 *
 * @param delta Timing values from parent loop
 */
void Player::update(float delta) {
    Obstacle::update(delta);
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
        _sceneNode->setAngle(getAngle());
    }
}

void Player::applyForce() {
    if (!isActive()) {
        return;
    }
    
    // Orient the force with rotation.
    Vec4 netforce(_force.x,_force.y,0.0f,1.0f);
    Mat4::createRotationZ(getAngle(),&_affine);
    netforce *= _affine;
    
    // Apply force to the rocket BODY, not the rocket
    _body->ApplyForceToCenter(b2Vec2(netforce.x,netforce.y), true);
}

void Player::setDrawScale(float scale) {
    _drawscale = scale;
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
}
