
#include "NetworkController.h"
#include "NetworkData.h"
#include "Player.h"
#include "Orb.h"
#include "Egg.h"
#include <cugl/cugl.h>


namespace NetworkController {
    namespace {
        std::shared_ptr<cugl::CUNetworkConnection> network;
        std::string roomId;
        int lastNum = 999;
        std::shared_ptr<World> world;
        //Username would need to go from LoadingScene to GameScene so more convenient as a global variable
        std::string username;
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
    }

    void joinGame(std::string roomId) {
        network =
            std::make_shared<cugl::CUNetworkConnection>(cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, 6, 0), roomId);
        NetworkController::roomId = roomId;
        CULog("REACHED JOIN");
        CULog("%s", roomId.c_str());
        CULog("num players %d", network->getNumPlayers());
        CULog("total players %d", network->getTotalPlayers());
    }

    void setWorld(std::shared_ptr<World> w){
        world = w;
    }

    bool isHost(){
        return network->getPlayerID().value_or(-1) == 0;
    }

    std::optional<uint8_t> getPlayerId(){
        return network->getPlayerID();
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

    std::string getUsername() {
        return username;
    }

    void setUsername(std::string name) {
        username = name;
    }

    void receive(const std::function<void(const std::vector<uint8_t>&)>& dispatcher){
        network->receive(dispatcher);
    }

    void send(const std::vector<uint8_t>& msg){
        network->send(msg);
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

    void update(float timestep){
        network->receive([&](const std::vector<uint8_t> msg) {
            ND::NetworkData nd{};
            ND::fromBytes(nd, msg);
            switch(nd.packetType){
                case ND::NetworkData::POSITION_PACKET:
                    {
                        auto p = world->getPlayer(nd.positionData.playerId);
                        p->setPosition(nd.positionData.playerPos);
                        p->setLinearVelocity(nd.positionData.playerVelocity);
                    }
                    break;
                default:
                    break;
            }
        });
    }

    void sendPosition(){
        if(! getPlayerId().has_value()) return;
        auto p = world->getPlayer(getPlayerId().value());
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::POSITION_PACKET;
        nd.positionData.playerPos = p->getPosition();
        nd.positionData.playerVelocity = p->getLinearVelocity();
        nd.positionData.playerId = NetworkController::getPlayerId().value();
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

}



