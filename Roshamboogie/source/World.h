//
//  World.h
//  Roshamboogie
//
//  Created by Joshua Kaplan on 3/23/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef World_h
#define World_h

#include <cugl/cugl.h>
#include "Player.h"
#include "Orb.h"
#include "SwapStation.h"
#include "Egg.h"

class World {
private:
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The Box2D world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _physicsWorld;
    
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _worldNode;
    std::shared_ptr<cugl::scene2::SceneNode> _debugNode;
    
    std::vector<std::shared_ptr<Player>> _players;
    
    std::vector<std::shared_ptr<Orb>> _orbs;
    
    std::vector<std::shared_ptr<SwapStation>> _swapStations;
    
    std::vector<std::shared_ptr<Egg>> _eggs;
    
    float _scale;
    uint8_t _numPlayers;
    uint8_t _currOrbCount;

public:
#pragma mark -
#pragma mark Constructors
    World(){}
    ~World() { dispose(); }
    void dispose();
    
    bool init(int width, int height);
    
    static std::shared_ptr<World> alloc(const std::shared_ptr<cugl::AssetManager>& assets, int width, int height, float scale, uint8_t numPlayers){
        std::shared_ptr<World> result = std::make_shared<World>();
        result->_assets = assets;
        result->_scale = scale;
        result->_numPlayers = numPlayers;
        return (result->init(width, height) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Gameplay Handling
    void reset();
    
#pragma mark -
#pragma mark Getters and setters
    std::shared_ptr<Player> getPlayer(int id){
        return _players[id];
    }
    
    std::vector<std::shared_ptr<Player>> getPlayers(){
        return _players;
    }
    
    std::shared_ptr<Orb> getOrb(int id){
        return _orbs[id];
    }
    
    std::vector<std::shared_ptr<Orb>> getOrbs(){
        return _orbs;
    }
    
    uint8_t getCurrOrbCount() {
        return _currOrbCount;
    }
    
    void setOrbCount(int c) {
        _currOrbCount = c;
    }
    
    void addOrb(Vec2 pos);
    
    float getScale() { return _scale; }
    
    
    std::shared_ptr<Egg> getEgg(int id){
        return _eggs[id];
    }
    
    std::shared_ptr<SwapStation> getSwapStation(int id){
        return _swapStations[id];
    }
    
    std::shared_ptr<cugl::scene2::SceneNode> getSceneNode(){
        return _worldNode;
    }
    
    std::shared_ptr<cugl::physics2::ObstacleWorld> getPhysicsWorld(){
        return _physicsWorld;
    }
    
    void setDebugNode(const std::shared_ptr<cugl::scene2::SceneNode> & debugNode){
        _debugNode = debugNode;
    }
};


#endif /* World_h */
