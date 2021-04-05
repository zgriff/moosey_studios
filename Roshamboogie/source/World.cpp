//
//  World.cpp
//  Roshamboogie
//
//  Created by Joshua Kaplan on 3/25/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "World.h"

/** The initial player position */
float PLAYER_POS[] = {24,  4};

void World::dispose(){
    _players.clear();
    _eggs.clear();
    _orbs.clear();
    _swapStations.clear();
    _worldNode = nullptr;
    _debugNode = nullptr;
    _physicsWorld = nullptr;
    _assets = nullptr;
}

bool World::init(int width, int height){
    Rect rect(0,0,width,height);
    _physicsWorld = physics2::ObstacleWorld::alloc(rect,Vec2::ZERO);
    _worldNode = scene2::SceneNode::alloc();
    _worldNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _currOrbCount = 3;
    
    return true;
}

void World::reset(){
    _physicsWorld->clear();
    _worldNode->removeAllChildren();
    
    auto playerTexture = _assets->get<Texture>("player");
    auto orbTexture = _assets->get<Texture>("photon");
    auto swapStTexture = _assets->get<Texture>("swapstation");
    auto eggTexture = _assets->get<Texture>("target");

    Vec2 playerPos = ((Vec2)PLAYER_POS);
    Size playerSize(1, 2);
    for(int i = 0; i < 3; ++i){ //TODO: Change to _numPlayers once that's locked in
        auto player = Player::alloc(playerPos, playerSize, Element::Water);
        _physicsWorld->addObstacle(player);
        player->setTextures(playerTexture);
        player->setID(i);
        player->setDrawScale(_scale);
        player->setDebugColor(Color4::YELLOW);
        player->setDebugScene(_debugNode);
        player->allocUsernameNode(_assets->get<Font>("username"));
        _players.push_back(player);
        _worldNode->addChild(player->getSceneNode());
    }
    
    //creating the three orbs
    auto fireOrb = Orb::alloc(Vec2(4,4));
    _physicsWorld->addObstacle(fireOrb);
    fireOrb->setTextures(orbTexture);
    fireOrb->setDrawScale(_scale);
    fireOrb->setDebugColor(Color4::YELLOW);
    fireOrb->setDebugScene(_debugNode);
    fireOrb->setID(0);
    _orbs.push_back(fireOrb);

    auto waterOrb = Orb::alloc(Vec2(20,8));
    _physicsWorld->addObstacle(waterOrb);
    waterOrb->setTextures(orbTexture);
    waterOrb->setDrawScale(_scale);
    waterOrb->setDebugColor(Color4::YELLOW);
    waterOrb->setDebugScene(_debugNode);
    waterOrb->setID(1);
    _orbs.push_back(waterOrb);
    
    auto grassOrb = Orb::alloc(Vec2(10,12));
    _physicsWorld->addObstacle(grassOrb);
    grassOrb->setTextures(orbTexture);
    grassOrb->setDrawScale(_scale);
    grassOrb->setDebugColor(Color4::YELLOW);
    grassOrb->setDebugScene(_debugNode);
    grassOrb->setID(2);
    _orbs.push_back(grassOrb);
    
    
    Vec2 swapStPos = Vec2(8,8);
    Size swapStSize(swapStTexture->getSize() / _scale);
    auto swapStation = SwapStation::alloc(swapStPos, swapStSize);
    _physicsWorld->addObstacle(swapStation);
    swapStation->setTextures(swapStTexture);
    swapStation->setDrawScale(_scale);
    swapStation->setActive(true);
    swapStation->setDebugColor(Color4::YELLOW);
    swapStation->setDebugScene(_debugNode);
    swapStation->setID(0);
    _swapStations.push_back(swapStation);
    
    Vec2 eggPos = Vec2(14,14);
    Size eggSize(eggTexture->getSize() / _scale);
    auto egg = Egg::alloc(eggPos, eggSize);
    _physicsWorld->addObstacle(egg);
    egg->setTextures(eggTexture);
    egg->setDrawScale(_scale);
    egg->setActive(true);
    egg->setDebugColor(Color4::YELLOW);
    egg->setDebugScene(_debugNode);
    egg->setID(0);
    _eggs.push_back(egg);
    
    _worldNode->addChild(fireOrb->getSceneNode());
    _worldNode->addChild(waterOrb->getSceneNode());
    _worldNode->addChild(grassOrb->getSceneNode());
    _worldNode->addChild(swapStation->getSceneNode());
    _worldNode->addChild(egg->getSceneNode());
}

//creates a new orb in the world with the given pos
void World::addOrb(Vec2 pos) {
    auto orbTexture = _assets->get<Texture>("photon");
    auto orb = Orb::alloc(pos);
    _physicsWorld->addObstacle(orb);
    orb->setTextures(orbTexture);
    orb->setDrawScale(_scale);
    orb->setDebugColor(Color4::YELLOW);
    orb->setDebugScene(_debugNode);
    orb->setID(_currOrbCount);
    _orbs.push_back(orb);
    _currOrbCount++;
    _worldNode->addChild(orb->getSceneNode());

}
