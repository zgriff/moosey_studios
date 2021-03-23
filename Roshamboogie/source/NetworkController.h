#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__

#include <cugl/cugl.h>

namespace NetworkController {

	void createGame();

	void joinGame(std::string roomId);
    
    /** isHost is true if the player is a host of a game and false otherwise */
    bool isHost();
};

#endif
