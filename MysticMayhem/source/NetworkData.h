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
        ELEMENT_CHANGE, PROJECTILE_FIRED, PROJECTILE_GONE,
        SET_USERNAME, SET_MAP_NUMBER, CLIENT_READY, CLIENT_UNREADY, HOST_STARTGAME
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
            bool dropEgg;
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
            float angle;
            uint8_t playerId;
        } positionData;
        struct {
            uint8_t playerId;
            Element newElement;
        } elementChangeData;
        struct {
            uint8_t projectileId;
            cugl::Vec2 projectilePos;
            float projectileAngle;
            Element preyElement;
        } projectileFiredData;
        struct {
            uint8_t projectileId;
        } projectileGoneData;
        struct {
            uint8_t playerId;
            int username_length;
            char * username;
        } setUsernameData;
        struct {
            uint8_t mapNumber;
        } setMapNumber;
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
