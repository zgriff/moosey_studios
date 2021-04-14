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
    enum PacketType {
        TAG_PACKET, ORB_RESPAWN, EGG_RESPAWN, ORB_CAPTURED, EGG_CAPTURED, EGG_HATCHED, SWAP_PACKET, POSITION_PACKET,
        CLIENT_READY, CLIENT_UNREADY, HOST_STARTGAME
    };
    uint8_t packetType;
    union {
        struct {
            uint8_t player_id;
        } readyData;
        struct {
            uint8_t taggedId;
            uint8_t taggerId;
            time_t timestamp;
        } tagData;
        struct {
            uint8_t orbId;
            cugl::Vec2 position;
        } orbRespawnData;
        struct {
            uint8_t eggId;
            cugl::Vec2 position;
        } eggRespawnData;
        struct {
            uint8_t playerId;
            uint8_t eggId;
        } eggCapData;
        struct {
            uint8_t playerId;
            uint8_t eggId;
        } eggHatchData;
        struct {
            uint8_t orbId;
            uint8_t playerId;
        } orbCapData;
        struct {
            uint8_t swapId;
            uint8_t playerId;
            Element newElement;
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
