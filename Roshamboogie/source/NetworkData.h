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

#define MAX_PLAYERS 6
#define MAX_ORBS 20
#define MAX_SWAP_STATIONS 5

struct PlayerData {
    cugl::Vec2 playerPos;
    cugl::Vec2 playerVelocity;
    Element e;
    int score;
    int playerId; //could get rid of this
    bool isHoldingEgg;
    int powerLevel;
    bool wasTagged;
};

struct SwapStationData {
    int id;
    int cooldown;
};

struct OrbData {
    cugl::Vec2 orbPos;
};

struct NetworkData {
    bool type;
    union data {
        struct hostData {
            PlayerData players[MAX_PLAYERS];
            SwapStationData swapData[MAX_SWAP_STATIONS];
            OrbData orbData[MAX_ORBS];
        };
        struct clientData {
            cugl::Vec2 playerPos;
            cugl::Vec2 playerVelocity;
            int playerId;
        };
    };
}
;
#endif /* NetworkData_h */
