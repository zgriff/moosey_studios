#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__

#include <cugl/cugl.h>

namespace NetworkController {

	void createGame();

	void joinGame(std::string roomId);
    
    /** isHost is true if the player is a host of a game and false otherwise */
    bool isHost();

    std::string getRoomId();

    uint8_t getNumPlayers();

    void step();

    void send(const std::vector<uint8_t>& msg);

    void receive(const std::function<void(const std::vector<uint8_t>&)>& dispatcher);

    std::string getUsername();

    void setUsername(std::string name);

};

#endif
