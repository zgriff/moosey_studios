
#include "NetworkController.h"
#include <cugl/cugl.h>


namespace NetworkController {
    namespace {
        std::shared_ptr<cugl::CUNetworkConnection> network;
        std::string roomId;
        bool _isHost;
        int lastNum = 999;
    }

    /** IP of the NAT punchthrough server */
    constexpr auto SERVER_ADDRESS = "34.74.68.73";
    /** Port of the NAT punchthrough server */
    constexpr uint16_t SERVER_PORT = 61111;

    void createGame() {
        network =
            std::make_shared<cugl::CUNetworkConnection>(
                cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0));
        CULog("REACHED CREATE");
        _isHost = true;
    }

    void joinGame(std::string roomId) {
        network =
            std::make_shared<cugl::CUNetworkConnection>(cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0), roomId);
        _isHost = false;
        NetworkController::roomId = roomId;
        CULog("REACHED JOIN");
        CULog(roomId.c_str());
        CULog("num players %d", network->getNumPlayers());
        CULog("total players %d", network->getTotalPlayers());
    }

    bool isHost(){
        return _isHost;
    }

    std::string getRoomId() {
        if (roomId == "") {
            roomId = network->getRoomID();
        }
        //CULog("Room ID is: %s", roomId.c_str());
        return roomId;
    }

    uint8_t getNumPlayers() {
        return network->getNumPlayers();
    }

    void step() {
        auto* k = cugl::Input::get<cugl::Keyboard>();
        if (k->keyPressed(cugl::KeyCode::SPACE)) {
            CULog("Sending");
            std::vector<uint8_t> msg = { 1,2,3,4 };
            network->send(msg);
        }
        network->receive([&](const std::vector<uint8_t> msg) {
            CULog("Received message of length %lu", msg.size());
            for (const auto& d : msg) {
                CULog("%d", d);
            }
            });
        if (network->getNumPlayers() != lastNum) {
            lastNum = network->getNumPlayers();
            CULog("Num players %d, total players %d", network->getNumPlayers(), network->getTotalPlayers());
            CULog("Room ID %s", network->getRoomID().c_str());
            if (network->getPlayerID().has_value()) {
                CULog("Player ID %d", *network->getPlayerID());
            }
        }
    }

}



