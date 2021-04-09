#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__

#include <cugl/cugl.h>
#include "World.h"

#define NETWORK_FRAMERATE 6

namespace NetworkController {

	void createGame();

	void joinGame(std::string roomId);
    
    /** isHost is true if the player is a host of a game and false otherwise */
    bool isHost();

    std::optional<uint8_t> getPlayerId();

    std::string getRoomId();

    uint8_t getNumPlayers();

    void step();
    
    /** Updates world states based on network packets. Should call at beginning of gamescene update method*/
    void update(float timestep);

    /** Send the current position and velocity over the network*/
    void sendPosition();

    void sendOrbCaptured(int orbId, int playerId);
    void sendPlayerColorSwap(int playerId, Element newElement, int swapId);
    void sendEggCollected(int playerId, int eggId);
    void sendOrbRespawn(int orbId, Vec2 orbPosition);
    void sendTag(int taggedId, int taggerId, time_t timestamp);
    //Give the network controller a reference to the world
    void setWorld(std::shared_ptr<World> w);

    std::string getUsername();

    void setUsername(std::string name);

};

#endif
