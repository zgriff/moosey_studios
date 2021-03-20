#ifndef __MATCHMAKING_CONTROLLER_H__
#define __MATCHMAKING_CONTROLLER_H__

#include <cugl/cugl.h>

class MatchmakingController {
private:
	std::string roomId;

public:

	void createGame();

	void joinGame();
};

#endif