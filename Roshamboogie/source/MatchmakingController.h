#ifndef __MATCHMAKING_CONTROLLER_H__
#define __MATCHMAKING_CONTROLLER_H__

#include <cugl/cugl.h>

class MatchmakingController {
private:
	std::shared_ptr<cugl::CUNetworkConnection> _network;
	std::string _roomId;
	bool _isHost;

	MatchmakingController();
	~MatchmakingController() { dispose(); }

public:
	//Singleton
	static MatchmakingController* getInstance();

	void createGame();

	void joinGame(std::string roomId);

	bool init();

};

#endif