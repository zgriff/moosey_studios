//
//  NetworkData.h
//  Roshamboogie
//
//  Created by Joshua Kaplan on 3/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef NetworkData_h
#define NetworkData_h

#include <cugl/cugl.h>
#include "Element.h"
#include "Globals.h"

class NetworkData{
    using None = std::monostate;
    struct Ready {
        uint8_t player_id;
    };
    using Unready = Ready;
    struct Tag {
        uint8_t taggedId;
        uint8_t taggerId;
        time_t timestamp;
        bool dropEgg;
    };
    using StartGame = std::monostate;
    struct OrbRespawn {
        uint8_t orbId;
        cugl::Vec2 position;
    };
    struct EggRespawn {
        uint8_t eggId;
        cugl::Vec2 position;
    };
    struct EggCapture {
        uint8_t playerId;
        uint8_t eggId;
    };
    struct EggHatch {
        uint8_t playerId;
        uint8_t eggId;
    };
    struct OrbCapture {
        uint8_t orbId;
        uint8_t playerId;
    };
    struct Swap {
        uint8_t swapId;
        uint8_t playerId;
        Element newElement;
    };
    struct Position {
        cugl::Vec2 playerPos;
        cugl::Vec2 playerVelocity;
        uint8_t playerId;
    };
    struct ElementChange {
        uint8_t playerId;
        Element newElement;
    };
    struct ProjectileFired {
        uint8_t projectileId;
        cugl::Vec2 projectilePos;
        float projectileAngle;
        Element preyElement;
    };
    struct ProjectileGone {
        uint8_t projectileId;
    };

    std::variant<None, Ready, Unready, Tag, StartGame, OrbRespawn, EggRespawn, EggCapture, EggHatch, OrbCapture, Swap, Position, ElementChange, ProjectileFired, ProjectileGone> data;
    
    NetworkData(){}
        
    //convert the NetworkData to bytes
    std::vector<uint8_t> toBytes();

    //convert the src bytes to a NetworkData
    static NetworkData fromBytes(const std::vector<uint8_t>& src);

};


#endif /* NetworkData_h */
