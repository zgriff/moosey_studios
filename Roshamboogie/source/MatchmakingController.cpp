
#include "MatchmakingController.h"
#include <cugl/cugl.h>


/** IP of the NAT punchthrough server */
constexpr auto SERVER_ADDRESS = "34.74.68.73";
/** Port of the NAT punchthrough server */
constexpr uint16_t SERVER_PORT = 61111;

void MatchmakingController::createGame() {
	std::shared_ptr<cugl::CUNetworkConnection> network = 
		std::make_shared<cugl::CUNetworkConnection>(cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0));
	while (network->getRoomID() == "") {
		
	}
	roomId = network->getRoomID();
}

void MatchmakingController::joinGame(std::string roomId) {
	std::shared_ptr<cugl::CUNetworkConnection> network = 
		std::make_shared<cugl::CUNetworkConnection>(cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0), roomId);
}