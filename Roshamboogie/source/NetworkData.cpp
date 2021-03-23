//
//  NetworkData.cpp
//  Roshamboogie
//
//  Created by Joshua Kaplan on 3/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "NetworkData.h"
#include <vector>
#include "Globals.h"

#define TYPE_BITS 2
#define PLAYER_ID_BITS 3

uint64_t scratch;
int scratch_bits;
int offset;

//interpret uint32_t as uint8_t[4]
union ui32_to_ui8 {
    uint32_t ui32;
    uint8_t ui8[4];
};

void write32(std::vector<uint8_t> & buffer, uint32_t data){
    uint32_t marshalled = cugl::marshall(data);
    ui32_to_ui8 u;
    u.ui32 = marshalled;
    buffer.push_back(u.ui8[0]);
    buffer.push_back(u.ui8[1]);
    buffer.push_back(u.ui8[2]);
    buffer.push_back(u.ui8[3]);
}

void writeBits(std::vector<uint8_t> & buffer, uint32_t data, int numBits){
    scratch |= (data & ((1 << numBits) - 1)) << scratch_bits;
    scratch_bits += numBits;
    if(scratch_bits >= 32){
        write32(buffer,scratch & 0xFF);
        scratch >>= 32;
        scratch_bits -= 32;
    }
}

void writeFloat(std::vector<uint8_t> & buffer, float data){
    writeBits(buffer, static_cast<uint32_t>(data), 32);
}

void writeVec2(std::vector<uint8_t> & buffer, cugl::Vec2 data){
    writeFloat(buffer, data.x);
    writeFloat(buffer, data.y);
}

//call at the end of writing data to make sure everything ends up in the buffer
void flush(std::vector<uint8_t> & buffer){
    write32(buffer,scratch & 0xFF);
    scratch = 0;
    scratch_bits = 0;
}



//convert the bytes to a NetworkData struct, putting the result in dest
//returns true on success, false on failure (if data is corrupted)
bool fromBytes(struct NetworkData & dest, const std::vector<uint8_t>& bytes){
//    dest.packetType = readBits(bytes, TYPE_BITS);
    CULog("not implemented yet, sorry\n");
    return false;
}

//convert the NetworkData struct to bytes, putting the result in dest
//returns true on success, false on failure (if the data is corrupted)
bool toBytes(std::vector<uint8_t> & dest, const struct NetworkData & src){
    scratch = 0;
    scratch_bits = 0;
    writeBits(dest, src.packetType, TYPE_BITS);
    switch(src.packetType){
        case NetworkData::WORLD_DATA:
            CULog("not implemented yet, sorry\n");
            //TODO
            break;
        case NetworkData::HOST_PACKET:
            writeVec2(dest, src.hostData.hostPos);
            writeVec2(dest, src.hostData.hostVelocity);
            if(src.hostData.num_players <= globals::MAX_PLAYERS
               && src.hostData.num_players >= globals::MIN_PLAYERS){
                writeBits(dest, src.hostData.num_players, 8);
            } else {
                return false;
            }
            break;
            //TODO: finish the rest of host data.
//            uint8_t num_players;
//            PlayerData players[MAX_PLAYERS];
//            SwapStationData swapData[MAX_SWAP_STATIONS];
//            OrbData orbData[MAX_ORBS];
        case NetworkData::CLIENT_PACKET:
            writeVec2(dest, src.clientData.playerPos);
            writeVec2(dest, src.clientData.playerVelocity);
            writeBits(dest, src.clientData.playerId, PLAYER_ID_BITS);
            break;
        default:
            return false;
    }
    flush(dest);
    return true;
}
