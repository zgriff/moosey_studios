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

using namespace cugl;

void SwapStation::setTextures(const std::shared_ptr<Texture>& swapText) {

    _sceneNode = scene2::PolygonNode::allocWithTexture(swapText);
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _texture = swapText;
    _body->SetUserData(this);

}

bool SwapStation::init(const cugl::Vec2 pos, const cugl::Size size) {
    if(physics2::BoxObstacle::init(pos,size)){
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





