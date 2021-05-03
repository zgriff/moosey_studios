//
//  SwapStation.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//


#include <cugl/cugl.h>
#include "SwapStation.h"
#include "Element.h"

///** Number of rows in the image filmstrip */
//#define SWAPST_ROWS       1
///** Number of columns in this image filmstrip */
//#define SWAPST_COLS       2
///** Number of elements in this image filmstrip */
//#define SWAPST_FRAMES     2
#define SWAPST_RADIUS     0.9

using namespace cugl;

void SwapStation::setTextures(const std::shared_ptr<Texture>& swapText) {

    _sceneNode = scene2::PolygonNode::allocWithTexture(swapText);
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
//    _animationNode = scene2::AnimationNode::alloc(swapText, SWAPST_ROWS, SWAPST_COLS, SWAPST_FRAMES);
//    _animationNode->setAnchor(Vec2::ANCHOR_CENTER);
//    _animationNode->setFrame(0);
//    _animationNode->setPosition(0,0);
//    _sceneNode->addChild(_animationNode);
    _texture = swapText;
    _body->SetUserData(this);
}

bool SwapStation::init(const cugl::Vec2 pos) {
    if(physics2::WheelObstacle::init(pos, SWAPST_RADIUS)){
        setSensor(true);
        setName("swapstation");
        return true;
    }
    return false;
}

void SwapStation::dispose(){
    _sceneNode = nullptr;
    _texture = nullptr;
}

void SwapStation::setDrawScale(float scale) {
    _drawscale = scale;
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
}

/* @param delta Timing values from parent loop
*/
void SwapStation::update(float delta) {
    Obstacle::update(delta);
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
    if (time(NULL) - _lastUsed >= _coolDownSecs) {
        _active = true;
        _sceneNode->setColor(Color4(255, 255, 255, 255));
    }
    else {
        _active = false;
        _sceneNode->setColor(Color4(255, 255, 255, 100));
    }
}
