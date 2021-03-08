//
//  Ship.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "Player.h"
#include "Element.h"

using namespace cugl;

// Default physics values
/** The density of this rocket */
#define DEFAULT_DENSITY 1.0f
/** The friction of this rocket */
#define DEFAULT_FRICTION 0.1f
/** The restitution of this rocket */
#define DEFAULT_RESTITUTION 0.4f

#define SIGNUM(x)  ((x > 0) - (x < 0))



/**
 * Sets the textures for this ship.
 *
 * The two textures are the ship texture and the target texture. The
 * scene graph node associated with this ship is nullptr until these
 * values are set.
 *
 * @param ship      The texture for the ship filmstrip
 * @param target    The texture for the ship target
 */
void Player::setTextures(const std::shared_ptr<Texture>& ship) {

    _sceneNode = scene2::PolygonNode::allocWithTexture(ship);
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _texture = ship;
    _body->SetUserData(this);
}

void Player::setElement(Element e){
    element = e;
}

/**
 * Disposes the ship, releasing all resources.
 */
void Player::dispose() {
    // Garbage collect
    _sceneNode = nullptr;
    _texture = nullptr;
}

/**
 * Initializes a new ship at the given location with the given facing.
 *
 * This method does NOT create a scene graph node for this ship.  You
 * must call setTextures for that.
 *
 * @param x The initial x-coordinate of the center
 * @param y The initial y-coordinate of the center
 * @param ang The initial angle of rotation
 *
 * @return true if the initialization was successful
 */
bool Player::init(const cugl::Vec2 pos, const cugl::Size size) {
<<<<<<< HEAD
    if(physics2::BoxObstacle::init(pos,size)){
        std::string name("player");
        setName(name);
        setDensity(DEFAULT_DENSITY);
        setFriction(DEFAULT_FRICTION);
        setRestitution(DEFAULT_RESTITUTION);
        setFixedRotation(true);
        setMoving(false);
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
    
//    if (!getMoving()) {
//        b2Vec2 force(-getDamping()*getVX(),-getDamping()*getVY());
//        _body->ApplyForce(force,_body->GetPosition(),true);
//    }
    
    if (fabs(getVX()) >= getMaxSpeed() && fabs(getVY()) >= getMaxSpeed()) {
        setVX(SIGNUM(getVX()*getMaxSpeed()));
        setVY(SIGNUM(getVY()*getMaxSpeed()));
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
