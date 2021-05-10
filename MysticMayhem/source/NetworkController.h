#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__

#include <cugl/cugl.h>
#include "World.h"
#include "LobbyScene.h"

#define NETWORK_FRAMERATE 6

namespace NetworkController {

    void createGame();

    void joinGame(std::string roomId);

    void destroyConn();
    
    /** isHost is true if the player is a host of a game and false otherwise */
    bool isHost();

    std::optional<uint8_t> getPlayerId();

    std::string getRoomId();

    uint8_t getNumPlayers();
    
    cugl::CUNetworkConnection::NetStatus getStatus();

    void step();
    
    /** Updates world states based on network packets. Should call at beginning of gamescene update method*/
    void update(float timestep);

    /** Send the current position and velocity over the network*/
    void sendPosition();

    void sendOrbCaptured(int orbId, int playerId);
    void sendPlayerColorSwap(int playerId, Element newElement, int swapId);
    void sendEggCollected(int playerId, int eggId);
    void sendEggHatched(int playerId, int eggId);
    void sendOrbRespawn(int orbId, Vec2 orbPosition);
    void sendElementChange(int playerId, Element newElement);
    void sendProjectileFired(int projectileId, Vec2 projectilePos, float projectileAngle, Element preyElement);
    void sendProjectileGone(int projectileId);
    void sendTag(int taggedId, int taggerId, time_t timestamp, bool dropEgg);
    void sendEggRespawn(int eggId, Vec2 eggPosition);
    void sendSetUsername(int playerId, string username);
    void sendSetCustomization(int playerId, int skin, int hat, int element);
    void sendSetMapSelected(int i);
    //Give the network controller a reference to the world
    void setWorld(std::shared_ptr<World> w);
    std::shared_ptr<World> getWorld();

    //void setLobbyScene(std::shared_ptr<LobbyScene> lobby);
    int getMapSelected();
    void setMapSelected(int i);

    std::unordered_map<int,std::tuple<int,int,int>> getCustomizations();

    std::string getUsername();
    std::string getUsername(int playerId);

    void setUsername(std::string name);
    void setUsername(std::string name, int playerId);

    void setReadyCallback(std::function<void(uint8_t, bool)> cb);
    void setStartCallback (std::function<void(void)> cb);

    //called by a client when they are ready
    void ready();
    //called by client if they were ready but are no longer ready
    void unready();
    //Called by the host when the game is ready to start
    void startGame();

};

#endif
