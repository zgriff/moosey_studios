
#include "MatchmakingController.h"
#include <cugl/cugl.h>


/** IP of the NAT punchthrough server */
constexpr auto SERVER_ADDRESS = "34.74.68.73";
/** Port of the NAT punchthrough server */
constexpr uint16_t SERVER_PORT = 61111;

void MatchmakingController::createGame() {
	network = std::make_shared<cugl::CUNetworkConnection>(ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0));
	while (network->getRoomID() == null) {
		
	}
	roomId = network->getRoomID();
}

void MatchmakingController::joinGame(std::string roomId) {
	network = std::make_shared<cugl::CUNetworkConnection>(ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0), roomId);
}