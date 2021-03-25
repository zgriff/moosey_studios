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

namespace ND{

struct NetworkData {
    enum PacketType {PLAYER_PACKET, ORB_PACKET, SWAP_PACKET, POSITION_PACKET};
    uint8_t packetType;
    union {
        struct {
            int playerId;
            Element e;
//            int score;
            bool isHoldingEgg;
//            int powerLevel;
//            bool wasTagged;
        } playerData;
        struct {
            int id;
            bool isCaptured;
            cugl::Vec2 orbPos;
        } orbData;
        struct {
            int id;
            bool isOnCooldown;
        } swapData;
        struct {
            cugl::Vec2 playerPos;
            cugl::Vec2 playerVelocity;
            uint8_t playerId;
        } positionData;
    };
    
};

//convert the bytes to a NetworkData struct, putting the result in dest
//returns true on success, false on failure (if data is corrupted)
bool toBytes(std::vector<uint8_t> & dest, const struct NetworkData & src);

//convert the NetworkData struct to bytes, putting the result in dest
//returns true on success, false on failure (if the data is corrupted)
bool fromBytes(struct NetworkData & dest, const std::vector<uint8_t>& src);
}
#endif /* NetworkData_h */
