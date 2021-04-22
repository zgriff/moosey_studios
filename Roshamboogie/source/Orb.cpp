//
//  Orb.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "Orb.h"
#include "Element.h"
#include <random>

#define ORB_RADIUS 0.6
/** Number of rows in the image filmstrip */
#define ORB_ROWS       3
/** Number of columns in this image filmstrip */
#define ORB_COLS       7
/** Number of elements in this image filmstrip */
#define ORB_FRAMES     20

using namespace cugl;

void Orb::setTextures(const std::shared_ptr<Texture>& orb) {

    _sceneNode = scene2::PolygonNode::alloc();
    _animationNode = scene2::AnimationNode::alloc(orb, ORB_ROWS, ORB_COLS, ORB_FRAMES);
    _animationNode->setAnchor(Vec2::ANCHOR_CENTER);
//        _animationNode->setFrame(0);
    _animationNode->setPosition(0,0);
    _sceneNode->addChild(_animationNode);
//    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _texture = orb;
    _body->SetUserData(this);
}

bool Orb::init(Vec2 pos){
    bool success = physics2::WheelObstacle::init(pos, ORB_RADIUS);
    if(success){
        setSensor(true);
        setBodyType(b2_staticBody);
        setName("orb");
        _collected = false;
    }
    return success;
}


void Orb::dispose(){
    if(_sceneNode != nullptr){
        _sceneNode->dispose();
        _sceneNode = nullptr;
    }
    _texture = nullptr;
}

void Orb::setDrawScale(float scale) {
    _drawscale = scale;
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
}

void Orb::update(float delta) {
    Obstacle::update(delta);
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
        _sceneNode->setAngle(getAngle());
    }
    if (_collected){
        _sceneNode->setVisible(false);
    }
    else {
        _sceneNode->setVisible(true);
    }

    //TODO: fix orb animation
    for (int i = 0; i < ORB_FRAMES; i++) {
        _animationNode->setFrame(i);
    }
}

//TODO: delete since spawning of orbs is handled by spawncontroller now
void Orb::respawn() {
    std::random_device r; //TODO: move to only initialize once
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> rand_int(1, 31);
    std::uniform_int_distribution<int> rand_int2(1, 17);
    setPosition(rand_int(e1), rand_int2(e1));
}
