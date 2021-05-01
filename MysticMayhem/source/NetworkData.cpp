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
//TODO: generate these from global maximums
#define TYPE_BITS 4
#define PLAYER_ID_BITS 3
#define SWAP_ID_BITS 4
#define ORB_ID_BITS 5
#define EGG_ID_BITS 3
#define MAP_NUMBER_BITS 3

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

union ui64_to_ui8 {
    uint64_t ui64;
    uint8_t ui8[8];
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

void writeBool(std::vector<uint8_t> & buffer, bool b){
    writeBits(buffer, b, 1);
}

void writeElement(std::vector<uint8_t> & buffer, Element e){
    writeBits(buffer, e, 3);
}

void writeTimestamp(std::vector<uint8_t> & buffer, time_t timestamp){
    uint64_t tmp = static_cast<uint64_t>(timestamp);
    uint64_t marshalled = cugl::marshall(tmp);
    ui64_to_ui8 u;
    u.ui64 = marshalled;
    writeBits(buffer, u.ui8[0], 8);
    writeBits(buffer, u.ui8[1], 8);
    writeBits(buffer, u.ui8[2], 8);
    writeBits(buffer, u.ui8[3], 8);
    writeBits(buffer, u.ui8[4], 8);
    writeBits(buffer, u.ui8[5], 8);
    writeBits(buffer, u.ui8[6], 8);
    writeBits(buffer, u.ui8[7], 8);
}

void writeString(std::vector<uint8_t>& buffer, int length, char s[]) {
    // supports string up to length 16
    writeBits(buffer, length, 4);
    for (int i = 0; i < length; i++) {
        // can optimize if we only want to have alphanumeric chars but then we will need custom char to int conversion
        uint8_t u = s[i];
        writeBits(buffer, u, 8);
    }
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

time_t readTimestamp(const std::vector<uint8_t>& bytes){
    ui64_to_ui8 u;
    u.ui8[0] = readBits(bytes, 8);
    u.ui8[1] = readBits(bytes, 8);
    u.ui8[2] = readBits(bytes, 8);
    u.ui8[3] = readBits(bytes, 8);
    u.ui8[4] = readBits(bytes, 8);
    u.ui8[5] = readBits(bytes, 8);
    u.ui8[6] = readBits(bytes, 8);
    u.ui8[7] = readBits(bytes, 8);
    uint64_t marshalled = cugl::marshall(u.ui64);
    return static_cast<time_t>(marshalled);
}

Element readElement(const std::vector<uint8_t>& bytes){
    uint32_t e = readBits(bytes, 3);
    return (Element) e;
}

bool readBool(const std::vector<uint8_t>& bytes){
    uint32_t b = readBits(bytes, 1);
    if(b == 0){
        return false;
    }
    return true;
}

int readStringLen(const std::vector<uint8_t>& bytes) {
    uint32_t len = readBits(bytes, 4);
    return len;
}

char * readString(const std::vector<uint8_t>& bytes, int len) {
    char * s = new char[globals::MAX_CHARS_IN_USERNAME];
    for (int i = 0; i < len; i++) {
        char c = readBits(bytes, 8);
        s[i] = c;
    }
    return s;
}



//convert the bytes to a NetworkData struct, putting the result in dest
//returns true on success, false on failure (if data is corrupted)
bool fromBytes(struct NetworkData & dest, const std::vector<uint8_t>& bytes){
    scratch = 0;
    scratch_bits = 0;
    byte_arr_index = 0;
    dest.packetType = readBits(bytes, TYPE_BITS);
    switch(dest.packetType){
        case NetworkData::TAG_PACKET:
            dest.tagData.taggedId = readBits(bytes, PLAYER_ID_BITS);
            dest.tagData.taggerId = readBits(bytes, PLAYER_ID_BITS);
            dest.tagData.timestamp = readTimestamp(bytes);
            dest.tagData.dropEgg = readBool(bytes);
            break;
        case NetworkData::ORB_CAPTURED:
            dest.orbCapData.orbId = readBits(bytes, ORB_ID_BITS);
            dest.orbCapData.playerId = readBits(bytes, PLAYER_ID_BITS);
            break;
        case NetworkData::EGG_CAPTURED:
            dest.eggCapData.playerId = readBits(bytes, PLAYER_ID_BITS);
            dest.eggCapData.eggId = readBits(bytes, EGG_ID_BITS);
            break;
        case NetworkData::SWAP_PACKET:
            dest.swapData.swapId = readBits(bytes, SWAP_ID_BITS);
            dest.swapData.playerId = readBits(bytes, PLAYER_ID_BITS);
            dest.swapData.newElement = readElement(bytes);
            break;
        case NetworkData::POSITION_PACKET:
            dest.positionData.playerPos = readVec2(bytes);
            dest.positionData.playerVelocity = readVec2(bytes);
            dest.positionData.angle = readFloat(bytes);
            dest.positionData.playerId = readBits(bytes, PLAYER_ID_BITS);
            break;
        case NetworkData::ORB_RESPAWN:
            dest.orbRespawnData.orbId = readBits(bytes, ORB_ID_BITS);
            dest.orbRespawnData.position = readVec2(bytes);
            break;
        case NetworkData::ELEMENT_CHANGE:
            dest.elementChangeData.playerId = readBits(bytes, PLAYER_ID_BITS);
            dest.elementChangeData.newElement = readElement(bytes);
            break;
        case NetworkData::PROJECTILE_FIRED:
            dest.projectileFiredData.projectileId = readBits(bytes, PLAYER_ID_BITS); //num of proj the same as players for now
            dest.projectileFiredData.projectilePos = readVec2(bytes);
            dest.projectileFiredData.projectileAngle = readFloat(bytes);
            dest.projectileFiredData.preyElement = readElement(bytes);
            break;
        case NetworkData::PROJECTILE_GONE:
            dest.projectileGoneData.projectileId = readBits(bytes, PLAYER_ID_BITS);
            break;
        case NetworkData::EGG_RESPAWN:
            dest.eggRespawnData.eggId = readBits(bytes, EGG_ID_BITS);
            dest.eggRespawnData.position = readVec2(bytes);
            break;
        case NetworkData::EGG_HATCHED:
            dest.eggHatchData.playerId = readBits(bytes, PLAYER_ID_BITS);
            dest.eggHatchData.eggId = readBits(bytes, EGG_ID_BITS);
            break;
        case NetworkData::CLIENT_READY:
        case NetworkData::CLIENT_UNREADY:
            dest.readyData.player_id = readBits(bytes, PLAYER_ID_BITS);
            break;
        case NetworkData::SET_USERNAME:
            dest.setUsernameData.playerId = readBits(bytes, PLAYER_ID_BITS);
            dest.setUsernameData.username_length = readStringLen(bytes);
            dest.setUsernameData.username = readString(bytes, dest.setUsernameData.username_length);
            break;
        case NetworkData::SET_MAP_NUMBER:
            dest.setMapNumber.mapNumber = readBits(bytes, MAP_NUMBER_BITS);
            break;
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
        case NetworkData::TAG_PACKET:
            writeBits(dest, src.tagData.taggedId, PLAYER_ID_BITS);
            writeBits(dest, src.tagData.taggerId, PLAYER_ID_BITS);
            writeTimestamp(dest, src.tagData.timestamp);
            writeBool(dest, src.tagData.dropEgg);
            break;
        case NetworkData::ORB_CAPTURED:
            writeBits(dest, src.orbCapData.orbId, ORB_ID_BITS);
            writeBits(dest, src.orbCapData.playerId, PLAYER_ID_BITS);
            break;
        case NetworkData::EGG_CAPTURED:
            writeBits(dest, src.eggCapData.playerId, PLAYER_ID_BITS);
            writeBits(dest, src.eggCapData.eggId, EGG_ID_BITS);
            break;
        case NetworkData::SWAP_PACKET:
            writeBits(dest, src.swapData.swapId, SWAP_ID_BITS);
            writeBits(dest, src.swapData.playerId, PLAYER_ID_BITS);
            writeElement(dest, src.swapData.newElement);
            break;
        case NetworkData::POSITION_PACKET:
            writeVec2(dest, src.positionData.playerPos);
            writeVec2(dest, src.positionData.playerVelocity);
            writeFloat(dest, src.positionData.angle);
            writeBits(dest, src.positionData.playerId, PLAYER_ID_BITS);
            break;
        case NetworkData::ORB_RESPAWN:
            writeBits(dest, src.orbRespawnData.orbId, ORB_ID_BITS);
            writeVec2(dest, src.orbRespawnData.position);
            break;
        case NetworkData::ELEMENT_CHANGE:
            writeBits(dest, src.elementChangeData.playerId, PLAYER_ID_BITS);
            writeElement(dest, src.elementChangeData.newElement);
            break;
        case NetworkData::PROJECTILE_FIRED:
            writeBits(dest, src.projectileFiredData.projectileId, PLAYER_ID_BITS);
            writeVec2(dest, src.projectileFiredData.projectilePos);
            writeFloat(dest, src.projectileFiredData.projectileAngle);
            writeElement(dest, src.projectileFiredData.preyElement);
            break;
        case NetworkData::PROJECTILE_GONE:
            writeBits(dest, src.projectileFiredData.projectileId, PLAYER_ID_BITS);
            break;
        case NetworkData::EGG_RESPAWN:
            writeBits(dest, src.eggRespawnData.eggId, EGG_ID_BITS);
            writeVec2(dest, src.eggRespawnData.position);
            break;
        case NetworkData::EGG_HATCHED:
            writeBits(dest, src.eggHatchData.playerId, PLAYER_ID_BITS);
            writeBits(dest, src.eggHatchData.eggId, EGG_ID_BITS);
            break;
        case NetworkData::CLIENT_READY:
        case NetworkData::CLIENT_UNREADY:
            writeBits(dest, src.readyData.player_id, PLAYER_ID_BITS);
            break;
        case NetworkData::SET_USERNAME:
            //CULog("reached here 2");
            writeBits(dest, src.setUsernameData.playerId, PLAYER_ID_BITS);
            writeString(dest, src.setUsernameData.username_length, src.setUsernameData.username);
            break;
        case NetworkData::SET_MAP_NUMBER:
            writeBits(dest, src.setMapNumber.mapNumber, MAP_NUMBER_BITS);
            break;
    }
    flush(dest);
    return true;
}
}
