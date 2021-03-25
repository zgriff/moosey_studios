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

namespace ND{
#define TYPE_BITS 2
#define PLAYER_ID_BITS 3

//writing
uint16_t scratch;
int scratch_bits;
//reading
int byte_arr_index;

//interpret uint32_t as uint8_t[4]
union ui32_to_ui8 {
    uint32_t ui32;
    uint8_t ui8[4];
};

union float_to_ui32 {
    uint32_t ui32;
    float f;
};

void writeByte(std::vector<uint8_t> & buffer, uint8_t data){
    buffer.push_back(data);
}

//1 <= numBits <= 8
void writeBits(std::vector<uint8_t> & buffer, uint8_t data, int numBits){
    uint16_t tmp = data & ((1 << numBits) - 1);
    scratch |= tmp << scratch_bits;
    scratch_bits += numBits;
    if(scratch_bits >= 8){
        writeByte(buffer,scratch & 0b11111111);
        scratch >>= 8;
        scratch_bits -= 8;
    }
}

void write32(std::vector<uint8_t> & buffer, uint32_t data){
    ui32_to_ui8 u;
    u.ui32 = data;
    //TODO: Check that this is the right order
    writeBits(buffer, u.ui8[0], 8);
    writeBits(buffer, u.ui8[1], 8);
    writeBits(buffer, u.ui8[2], 8);
    writeBits(buffer, u.ui8[3], 8);
}

void writeFloat(std::vector<uint8_t> & buffer, float data){
    float_to_ui32 ftu;
    ftu.f = data;
    uint32_t marshalled = cugl::marshall(ftu.ui32);
//    std::cout << marshalled << " write" << endl;
    write32(buffer, marshalled);
}

void writeVec2(std::vector<uint8_t> & buffer, cugl::Vec2 data){
    writeFloat(buffer, data.x);
    writeFloat(buffer, data.y);
}

//call at the end of writing data to make sure everything ends up in the buffer
void flush(std::vector<uint8_t> & buffer){
    writeByte(buffer,scratch & 0b11111111);
    scratch = 0;
    scratch_bits = 0;
}

//numBits must be between 1 and 32 inclusive
uint32_t readBits(const std::vector<uint8_t>& bytes, int numBits){
    uint32_t read = 0;
    int bits_read = 0;
    while(numBits > 0){
        //fill the scratch
        if(scratch_bits < numBits){
            uint8_t cur = bytes[byte_arr_index];
            uint16_t tmp = cur;
            scratch |= tmp << scratch_bits;
            scratch_bits += 8;
            ++byte_arr_index;
        }
        //grab bits
        if(numBits >= 8){
            uint32_t tmp = scratch & 0b11111111;
            read |= tmp << bits_read;
            scratch >>= 8;
            scratch_bits -= 8;
            numBits -= 8;
            bits_read += 8;
        }else{
            uint32_t tmp = scratch;
            read |= (tmp & ((1 << numBits) - 1)) << bits_read;
            scratch >>= numBits;
            scratch_bits -= numBits;
            numBits = 0;
        }
    }
    return read;
}

float readFloat(const std::vector<uint8_t>& bytes){
    uint32_t _x = readBits(bytes, 32);
    uint32_t marshalled = cugl::marshall(_x);
    float_to_ui32 ftu;
    ftu.ui32 = marshalled;
//    float x = static_cast<float>(marshalled);
    return ftu.f;
}


cugl::Vec2 readVec2(const std::vector<uint8_t>& bytes){
    float x = readFloat(bytes);
    float y = readFloat(bytes);
    return cugl::Vec2(x, y);
}



//convert the bytes to a NetworkData struct, putting the result in dest
//returns true on success, false on failure (if data is corrupted)
bool fromBytes(struct NetworkData & dest, const std::vector<uint8_t>& bytes){
    scratch = 0;
    scratch_bits = 0;
    byte_arr_index = 0;
    dest.packetType = readBits(bytes, TYPE_BITS);
    switch(dest.packetType){
        case NetworkData::WORLD_DATA:
            //TODO
            break;
        case NetworkData::HOST_PACKET:
            //TODO
            break;
        case NetworkData::CLIENT_PACKET:
//            dest.clientData.playerPos = readVec2(bytes);
//            dest.clientData.playerVelocity = readVec2(bytes);
            dest.clientData.playerPos_x = readFloat(bytes);
            dest.clientData.playerPos_y = readFloat(bytes);
            dest.clientData.playerVel_x = readFloat(bytes);
            dest.clientData.playerVel_y = readFloat(bytes);
            dest.clientData.playerId = readBits(bytes, PLAYER_ID_BITS);
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
            //TODO
            break;
        case NetworkData::HOST_PACKET:
//            writeVec2(dest, src.hostData.hostPos);
//            writeVec2(dest, src.hostData.hostVelocity);
//            if(src.hostData.num_players <= globals::MAX_PLAYERS
//               && src.hostData.num_players >= globals::MIN_PLAYERS){
//                writeBits(dest, src.hostData.num_players, 8);
//            } else {
//                return false;
//            }
            break;
            //TODO: finish the rest of host data.
//            uint8_t num_players;
//            PlayerData players[MAX_PLAYERS];
//            SwapStationData swapData[MAX_SWAP_STATIONS];
//            OrbData orbData[MAX_ORBS];
        case NetworkData::CLIENT_PACKET:
//            writeVec2(dest, src.clientData.playerPos);
//            writeVec2(dest, src.clientData.playerVelocity);
            writeFloat(dest, src.clientData.playerPos_x);
            writeFloat(dest, src.clientData.playerPos_y);
            writeFloat(dest, src.clientData.playerVel_x);
            writeFloat(dest, src.clientData.playerVel_y);
            writeBits(dest, src.clientData.playerId, PLAYER_ID_BITS);
            break;
        default:
            return false;
    }
    flush(dest);
    return true;
}
}
