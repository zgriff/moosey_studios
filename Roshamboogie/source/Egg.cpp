//
//  Egg.cpp
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/17/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <stdio.h>
#include <cugl/cugl.h>
#include "Egg.h"

using namespace cugl;

void Egg::setTextures(const std::shared_ptr<Texture>& eggText) {
    _sceneNode = scene2::PolygonNode::allocWithTexture(eggText);
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _texture = eggText;
    _body->SetUserData(this);
    _hatched = false;

}

bool Egg::init(const cugl::Vec2 pos, const cugl::Size size) {
    if(physics2::BoxObstacle::init(pos,size)){
        setSensor(true);
        setName("egg");
        setInitPos(pos);
        _collected = false;
        _distanceWalked = 0;
        return true;
    }
    return false;
}

void Egg::dispose(){
    if(_sceneNode != nullptr){
        _sceneNode->dispose();
        _sceneNode = nullptr;
    }
    _texture = nullptr;
}

void Egg::setDrawScale(float scale) {
    _drawscale = scale;
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
}

void Egg::update(float delta) {
    Obstacle::update(delta);
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
    
    if (_collected) {
        _sceneNode->setVisible(false);
    }
    else {
        _sceneNode->setVisible(true);
    }
}
