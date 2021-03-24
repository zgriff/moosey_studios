//
//  Orb.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "Orb.h"
#include "Element.h"

#define ORB_RADIUS 1

using namespace cugl;

void Orb::setTextures(const std::shared_ptr<Texture>& orb) {

    _sceneNode = scene2::PolygonNode::allocWithTexture(orb);
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _texture = orb;
    _body->SetUserData(this);
    setElement();
}
bool Orb::init(Element el){
    Vec2 spawnLoc(4,4);
    bool success = physics2::WheelObstacle::init(spawnLoc, ORB_RADIUS);
    if(success){
        e = el;
        setSensor(true);
        setBodyType(b2_staticBody);
        setName("orb");
            
    }
    return success;
}

void Orb::setElement() {
    switch(e){ //TODO: change to texture when assets made
        case Element::Grass:
            _sceneNode->setColor(Color4(0, 255, 0));
            break;
        case Element::Fire:
            _sceneNode->setColor(Color4(255, 0, 0));
            break;
        case Element::Water:
            _sceneNode->setColor(Color4(0, 0, 255));
            break;
    }
}

void Orb::dispose(){
    _sceneNode = nullptr;
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
}
