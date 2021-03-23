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

struct PlayerData {
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
    enum PacketType {WORLD_DATA, HOST_PACKET, CLIENT_PACKET};
    uint8_t packetType;
    union {
        struct {
            int map;
            uint8_t num_players;
            uint8_t num_swaps;
            uint8_t num_eggs;
        } worldData;
        struct {
            cugl::Vec2 hostPos;
            cugl::Vec2 hostVelocity;
            uint8_t num_players;
            PlayerData players[globals::MAX_PLAYERS]; //TODO change to exact number of players if possible
            SwapStationData swapData[globals::MAX_SWAP_STATIONS];
            OrbData orbData[globals::MAX_ORBS];
        } hostData;
        struct {
            cugl::Vec2 playerPos;
            cugl::Vec2 playerVelocity;
            uint8_t playerId;
        } clientData;
    };
    
};

//convert the bytes to a NetworkData struct, putting the result in dest
//returns true on success, false on failure (if data is corrupted)
bool toBytes(std::vector<uint8_t> & dest, const struct NetworkData & src);

//convert the NetworkData struct to bytes, putting the result in dest
//returns true on success, false on failure (if the data is corrupted)
bool fromBytes(struct NetworkData & dest, const std::vector<uint8_t>& src);

#endif /* NetworkData_h */
