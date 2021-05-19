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
#include <chrono>

#define ORB_RADIUS 0.6
/** Number of rows in the image filmstrip */
#define ORB_ROWS       3
/** Number of columns in this image filmstrip */
#define ORB_COLS       7
/** Number of elements in this image filmstrip */
#define ORB_FRAMES     20
#define SHADOW_OFFSET   -40.0f

using namespace cugl;

void Orb::setTextures(const std::shared_ptr<Texture>& orb, const std::shared_ptr<Texture>& shadow) {

    _orbNode = scene2::PolygonNode::allocWithTexture(orb);
    _orbNode->setAnchor(Vec2::ANCHOR_CENTER);
    _orbNode->setPosition(0,0);
    
    _shadowNode = scene2::PolygonNode::allocWithTexture(shadow);
    _shadowNode->setPosition(0,SHADOW_OFFSET);
    _shadowNode->setColor(Color4(255, 255, 255, 100));
    
//    _animationNode = scene2::AnimationNode::alloc(orb, ORB_ROWS, ORB_COLS, ORB_FRAMES);
//    _animationNode->setAnchor(Vec2::ANCHOR_CENTER);
//        _animationNode->setFrame(0);
//    _animationNode->setPosition(0,0);
//    _sceneNode->addChild(_animationNode);
    _sceneNode = scene2::SceneNode::alloc();
//    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _texture = orb;
    _sceneNode->addChild(_orbNode);
    _sceneNode->addChild(_shadowNode);
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
//    for (int i = 0; i < ORB_FRAMES; i++) {
//        _animationNode->setFrame(i);
//    }
}
