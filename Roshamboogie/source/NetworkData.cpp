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

//writing
uint64_t scratch;
int scratch_bits;
//reading
int byte_arr_index;
//should always be between 0 and 7 inclusive
int byte_offset;


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

//numBits must be between 1 and 32 inclusive
uint32_t readBits(const std::vector<uint8_t>& bytes, int numBits){
    uint32_t read = 0;
    int bits_read = 0;
    while(numBits > 0){
        read <<= bits_read;
        uint8_t cur = bytes[byte_arr_index];
        cur >>= byte_offset;
        if(numBits >= (8 - byte_offset)){
            read |= cur;
            byte_offset = 0;
            ++byte_arr_index;
            numBits -= (8 - byte_offset);
            bits_read += 8 - byte_offset;
        }else{
            uint8_t mask = 0; //TODO: fix
            read |= (cur & mask);
            byte_offset += numBits;
            numBits = 0;
        }
    }
    return read;
}


cugl::Vec2 readVec2(const std::vector<uint8_t>& bytes){
    uint32_t _x = readBits(bytes, 32);
    uint32_t _y = readBits(bytes, 32);
    float x = static_cast<float>(_x);
    float y = static_cast<float>(_y);
    return cugl::Vec2(x, y);
}



//convert the bytes to a NetworkData struct, putting the result in dest
//returns true on success, false on failure (if data is corrupted)
bool fromBytes(struct NetworkData & dest, const std::vector<uint8_t>& bytes){
    byte_arr_index = 0;
    byte_offset = 0;
    dest.packetType = readBits(bytes, TYPE_BITS);
    switch(dest.packetType){
        case NetworkData::WORLD_DATA:
            CULog("not implemented yet, sorry\n");
            //TODO
            break;
        case NetworkData::HOST_PACKET:
            CULog("not implemented yet, sorry\n");
            //TODO
            break;
        case NetworkData::CLIENT_PACKET:
            dest.clientData.playerPos = readVec2(bytes);
            dest.clientData.playerVelocity = readVec2(bytes);
            dest.clientData.playerId = readBits(bytes, 8);
            break;
        default:
            return false;
    }
    return true;
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
