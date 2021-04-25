//
//  Booster.cpp
//  Roshamboogie
//
//  Created by Thor Andreassen on 4/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//


#include <cugl/cugl.h>
#include "Booster.h"

/** Number of rows in the image filmstrip */
#define BOOST_ROWS       1
/** Number of columns in this image filmstrip */
#define BOOST_COLS       1
/** Number of elements in this image filmstrip */
#define BOOST_FRAMES     1
#define BOOST_SIDE_LEN     1.0f

using namespace cugl;

void Booster::setTextures(const std::shared_ptr<Texture>& boostText) {

    _sceneNode = scene2::PolygonNode::alloc();
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _animationNode = scene2::AnimationNode::alloc(boostText, BOOST_ROWS, BOOST_COLS, BOOST_FRAMES);
    _animationNode->setAnchor(Vec2::ANCHOR_CENTER);
//    _animationNode->setFrame(0);
    _animationNode->setPosition(0,0);
    _sceneNode->addChild(_animationNode);
    _texture = boostText;
    _body->SetUserData(this);
}

bool Booster::init(const cugl::Vec2 pos) {
    if(physics2::BoxObstacle::init(pos, Size(BOOST_SIDE_LEN, BOOST_SIDE_LEN))){
        setSensor(true);
        setName("booster");
        return true;
    }
    return false;
}

void Booster::dispose(){
    _sceneNode = nullptr;
    _texture = nullptr;
}

void Booster::setDrawScale(float scale) {
    _drawscale = scale;
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
}

/* @param delta Timing values from parent loop
*/
void Booster::update(float delta) {
    Obstacle::update(delta);
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
    if (time(NULL) - _lastUsed >= _coolDownSecs) {
        _active = true;
        _animationNode->setFrame(0);
    }
    else {
        _active = false;
        _animationNode->setFrame(0); //TODO: fix. this was throwing an error when it was on 1. I'm guessing the asset doesn't exist
    }
}
