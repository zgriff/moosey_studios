
#include "MatchmakingController.h"
#include <cugl/cugl.h>


/** IP of the NAT punchthrough server */
constexpr auto SERVER_ADDRESS = "34.74.68.73";
/** Port of the NAT punchthrough server */
constexpr uint16_t SERVER_PORT = 61111;

MatchmakingController::MatchmakingController() { }

bool MatchmakingController::init() { return true; }

void MatchmakingController::createGame() {
	_network = std::make_shared<cugl::CUNetworkConnection>(cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0));
	while (_network->getRoomID() == "") {
		
	}
	_roomId = _network->getRoomID();
}

void MatchmakingController::joinGame(std::string roomId) {
	_network = std::make_shared<cugl::CUNetworkConnection>(cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0), roomId);
}