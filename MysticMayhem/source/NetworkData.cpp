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

//TODO: generate these from global maximums
#define TYPE_BITS 5
#define PLAYER_ID_BITS 3
#define SWAP_ID_BITS 4
#define ORB_ID_BITS 5
#define EGG_ID_BITS 3
#define MAP_NUMBER_BITS 3

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

class ReadStream {
    int byte_arr_index = 0;
    int scratch_bits = 0;
    uint16_t scratch = 0;
    const std::vector<uint8_t> & buffer;
    
public:
    uint32_t readBits(int numBits){
        uint32_t read = 0;
        int bits_read = 0;
        while(numBits > 0){
            //fill the scratch
            if(scratch_bits < numBits){
                uint8_t cur = buffer[byte_arr_index];
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
    ReadStream( const std::vector<uint8_t> & buffer ) : buffer( buffer ) {}
    void serializeBits(uint8_t & data, int bits){
        data = readBits(bits);
    }
    void serializeFloat(float & data){
        uint32_t _x = readBits(32);
        uint32_t marshalled = cugl::marshall(_x);
        float_to_ui32 ftu;
        ftu.ui32 = marshalled;
    //    float x = static_cast<float>(marshalled);
        data = ftu.f;
    }
    
    void serializeVec2(cugl::Vec2 & data){
        serializeFloat(data.x);
        serializeFloat(data.y);
    }
    
    void serializeTimestamp(time_t & data){
        ui64_to_ui8 u;
        u.ui8[0] = readBits(8);
        u.ui8[1] = readBits(8);
        u.ui8[2] = readBits(8);
        u.ui8[3] = readBits(8);
        u.ui8[4] = readBits(8);
        u.ui8[5] = readBits(8);
        u.ui8[6] = readBits(8);
        u.ui8[7] = readBits(8);
        uint64_t marshalled = cugl::marshall(u.ui64);
        data = static_cast<time_t>(marshalled);
    }
    
    void serializeElement(Element & data){
        uint32_t e = readBits(3);
        data = (Element) e;
    }

    void serializeBool(bool & data){
        uint32_t b = readBits(1);
        if(b == 0){
            data = false;
        }
        else {
            data = true;
        }
    }
    
    void serializeString(std::string & data){
        data = std::string();
        while(true){
            char c = readBits(8);
            if(c == 0){
                break;
            }
            data.push_back(c);
        }
    }
};

class WriteStream {
    int byte_arr_index = 0;
    int scratch_bits = 0;
    uint16_t scratch = 0;
    std::vector<uint8_t> & buffer;
    
    void writeByte(uint8_t data){
        buffer.push_back(data);
    }
    void write32(uint32_t data){
        ui32_to_ui8 u;
        u.ui32 = data;
        //TODO: Check that this is the right order
        serializeBits(u.ui8[0], 8);
        serializeBits(u.ui8[1], 8);
        serializeBits(u.ui8[2], 8);
        serializeBits(u.ui8[3], 8);
    }
public:
    //call at the end of writing data to make sure everything ends up in the buffer
    void flush(){
        writeByte(scratch & 0b11111111);
        scratch = 0;
        scratch_bits = 0;
    }
    WriteStream( std::vector<uint8_t> & buffer ) : buffer( buffer ) {}
    //1 <= numBits <= 8
    
    void serializeBits(uint8_t data, int numBits){
        uint16_t tmp = data & ((1 << numBits) - 1);
        scratch |= tmp << scratch_bits;
        scratch_bits += numBits;
        if(scratch_bits >= 8){
            writeByte(scratch & 0b11111111);
            scratch >>= 8;
            scratch_bits -= 8;
        }
    }
    void serializeFloat(float data){
        float_to_ui32 ftu;
        ftu.f = data;
        uint32_t marshalled = cugl::marshall(ftu.ui32);
    //    std::cout << marshalled << " write" << endl;
        write32(marshalled);
    }
    
    void serializeVec2(cugl::Vec2 data){
        serializeFloat(data.x);
        serializeFloat(data.y);
    }
    
    void serializeBool(bool b){
        serializeBits(b, 1);
    }
    
    void serializeElement(Element e){
        serializeBits(e, 3);
    }
    
    void serializeTimestamp(time_t timestamp){
        uint64_t tmp = static_cast<uint64_t>(timestamp);
        uint64_t marshalled = cugl::marshall(tmp);
        ui64_to_ui8 u;
        u.ui64 = marshalled;
        serializeBits(u.ui8[0], 8);
        serializeBits(u.ui8[1], 8);
        serializeBits(u.ui8[2], 8);
        serializeBits(u.ui8[3], 8);
        serializeBits(u.ui8[4], 8);
        serializeBits(u.ui8[5], 8);
        serializeBits(u.ui8[6], 8);
        serializeBits(u.ui8[7], 8);
    }
    
    void serializeString(std::string & data){
        for (char const &c: data) {
            serializeBits(c, 8);
        }
    }
};


template <typename Stream>
struct NetworkData::Visitor {
    Stream & s;
    Visitor(Stream & st) : s(st){}
    void operator()(None & d) const {}
    void operator()(Ready & r) const {
        s.serializeBits(r.player_id, PLAYER_ID_BITS);
    }
    void operator()(Unready & r) const {
        s.serializeBits(r.player_id, PLAYER_ID_BITS);
    }
    void operator()(Tag & t) const {
        s.serializeBits(t.taggedId, PLAYER_ID_BITS);
        s.serializeBits(t.taggerId, PLAYER_ID_BITS);
        s.serializeTimestamp(t.timestamp);
        s.serializeBool(t.dropEgg);
    }
    void operator()(StartGame & s) const {}
    void operator()(OrbRespawn & o) const {
        s.serializeBits(o.orbId, ORB_ID_BITS);
        s.serializeVec2(o.position);
    }
    void operator()(EggRespawn & e) const {
        s.serializeBits(e.eggId, EGG_ID_BITS);
        s.serializeVec2(e.position);
    }
    void operator()(EggCapture & e) const {
        s.serializeBits(e.playerId, PLAYER_ID_BITS);
        s.serializeBits(e.eggId, EGG_ID_BITS);
    }
    void operator()(EggHatch & e) const {
        s.serializeBits(e.playerId, PLAYER_ID_BITS);
        s.serializeBits(e.eggId, EGG_ID_BITS);
    }
    void operator()(OrbCapture & o) const {
        s.serializeBits(o.orbId, ORB_ID_BITS);
        s.serializeBits(o.playerId, PLAYER_ID_BITS);
    }
    void operator()(Swap & sw) const {
        s.serializeBits(sw.swapId, SWAP_ID_BITS);
        s.serializeBits(sw.playerId, PLAYER_ID_BITS);
        s.serializeElement(sw.newElement);
    }
    void operator()(Position & p) const {
        s.serializeVec2(p.playerPos);
        s.serializeVec2(p.playerVelocity);
        s.serializeFloat(p.angle);
        s.serializeBits(p.playerId, PLAYER_ID_BITS);
    }
    void operator()(ElementChange & e) const {
        s.serializeBits(e.playerId, PLAYER_ID_BITS);
        s.serializeElement(e.newElement);
    }
    void operator()(ProjectileFired & p) const {
        s.serializeBits(p.projectileId, PLAYER_ID_BITS);
        s.serializeVec2(p.projectilePos);
        s.serializeFloat(p.projectileAngle);
        s.serializeElement(p.preyElement);
    }
    void operator()(ProjectileGone & p) const {
        s.serializeBits(p.projectileId, PLAYER_ID_BITS);
    }
    void operator()(SetUsername & d) const {
        s.serializeBits(d.playerId, PLAYER_ID_BITS);
        s.serializeString(d.username);
    }
    void operator()(SetCustomization & d) const {
        s.serializeBits(d.playerId, PLAYER_ID_BITS);
        s.serializeBits(d.skin, PLAYER_ID_BITS);
        s.serializeBits(d.hat, PLAYER_ID_BITS);
        s.serializeBits(d.element, PLAYER_ID_BITS);
    }
    void operator()(SetMap & d) const {
        s.serializeBits(d.mapNumber, MAP_NUMBER_BITS);
    }

};


////no clue how this works. taken from https://www.reddit.com/r/cpp/comments/f8cbzs/creating_stdvariant_based_on_index_at_runtime/
//template <typename... Ts>
//[[nodiscard]] std::variant<Ts...>
//expand_type(std::size_t i)
//{
//    static constexpr std::variant<Ts...> table[] = { Ts{ }... };
//    return table[i];
//}
////taken from https://stackoverflow.com/a/60567091
template <std::size_t I = 0>
void variant_from_index(NetworkData::DATA_T & v, std::size_t index) {
    if constexpr(I >= std::variant_size_v<NetworkData::DATA_T>)
        throw std::runtime_error{"Variant index " + std::to_string(I + index) + " out of bounds"};
    else {
        if(index == 0){
            v.emplace<I>();
        }else {
            variant_from_index<I + 1>(v, index - 1);
        }
    }
}

//convert the bytes to a NetworkData struct, putting the result in dest
//returns true on success, false on failure (if data is corrupted)
NetworkData NetworkData::fromBytes(const std::vector<uint8_t>& bytes){
    ReadStream s(bytes);
    int packetType = s.readBits(TYPE_BITS);
    
    NetworkData nd;
//    nd.data = expand_type(packetType);
//    nd.data = variant_from_index<NetworkData::DATA_T>(packetType);
    variant_from_index(nd.data, packetType);
//    nd.data.emplace<packetType>();
    std::visit(Visitor(s), nd.data);
    
    return nd;
}

//convert the NetworkData struct to bytes, putting the result in dest
//returns true on success, false on failure (if the data is corrupted)
std::vector<uint8_t> NetworkData::toBytes(){
    std::vector<uint8_t> out;
    WriteStream s(out);
    
    s.serializeBits(data.index(), TYPE_BITS);
    std::visit(Visitor(s), data);
    
    s.flush();
    return out;
}
