
#include "NetworkController.h"
#include "NetworkData.h"
#include "Player.h"
#include "Orb.h"
#include "Egg.h"
#include "Globals.h"
#include <cugl/cugl.h>
#include "MapConstants.h"
#include "SoundController.h"


namespace NetworkController {
    namespace {
        std::shared_ptr<cugl::CUNetworkConnection> network;
        std::string roomId;
        std::shared_ptr<World> world;
        //Username would need to go from LoadingScene to GameScene so more convenient as a global variable
        std::string username;
        //Networked usernames indexed by playerId
        array<std::string, 2> usernames = {"test", "test2"};
        int _networkFrame;
        int mapSelected;
        std::function<void(uint8_t, bool)> readyCallback;
        std::function<void(void)> startCallback;
    }

    /** IP of the NAT punchthrough server */
//    constexpr auto SERVER_ADDRESS = "35.208.113.51"; //ours;
    constexpr auto SERVER_ADDRESS = "34.74.68.73";
    /** Port of the NAT punchthrough server */
    constexpr uint16_t SERVER_PORT = 61111;

    void createGame() {
        network =
            std::make_shared<cugl::CUNetworkConnection>(
                cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, globals::MAX_PLAYERS, 0));
    }

    void joinGame(std::string roomId) {
        network =
            std::make_shared<cugl::CUNetworkConnection>(cugl::CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, globals::MAX_PLAYERS, 0), roomId);
        NetworkController::roomId = roomId;
        CULog("REACHED JOIN");
        CULog("%s", roomId.c_str());
        CULog("num players %d", network->getNumPlayers());
        CULog("total players %d", network->getTotalPlayers());
    }

    cugl::CUNetworkConnection::NetStatus getStatus(){
        if(network == nullptr){
            return cugl::CUNetworkConnection::NetStatus::Disconnected;
        }
        return network->getStatus();
    }

    void setWorld(std::shared_ptr<World> w){
        world = w;
    }

    std::shared_ptr<World> getWorld() {
        return world;
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

    int getMapSelected() {
        return mapSelected;
    }

    void setMapSelected(int i) {
        mapSelected = i;
    }

    void receive(const std::function<void(const std::vector<uint8_t>&)>& dispatcher){
        network->receive(dispatcher);
    }

    void send(const std::vector<uint8_t>& msg){
        network->send(msg);
    }

    void setReadyCallback(std::function<void(uint8_t, bool)> cb){
        readyCallback = cb;
    }

    void setStartCallback (std::function<void(void)> cb){
        startCallback = cb;
    }

    void step() {
        if(network == nullptr) return;
        network->receive([&](const std::vector<uint8_t> msg) {
            ND::NetworkData nd{};
            ND::fromBytes(nd, msg);
            switch(nd.packetType){
                case ND::NetworkData::HOST_STARTGAME:
                    startCallback();
                    break;
                    case ND::NetworkData::SET_MAP_NUMBER:
                    {
                        mapSelected = nd.setMapNumber.mapNumber;
                        CULog("set map selected to %d", mapSelected);
                    }
            }
//            CULog("Received message of length %lu", msg.size());
//            for (const auto& d : msg) {
//                CULog("%d", d);
//            }
        });
    }

    void update(float timestep){
        network->receive([&](const std::vector<uint8_t> msg) {
            ND::NetworkData nd{};
            ND::fromBytes(nd, msg);
            switch(nd.packetType){
                case ND::NetworkData::CLIENT_READY:
                    readyCallback(nd.readyData.player_id, true);
                    break;
                case ND::NetworkData::CLIENT_UNREADY:
                    readyCallback(nd.readyData.player_id, false);
                    break;
                case ND::NetworkData::HOST_STARTGAME:
                    startCallback();
                    break;
                case ND::NetworkData::SET_USERNAME:
                {
                    //usernames[nd.setUsernameData.playerId] = *nd.setUsernameData.username;
                    CULog("reached receive set username");
                    int id1 = nd.setUsernameData.playerId;
                    string username1(nd.setUsernameData.username, nd.setUsernameData.username_length);
                    delete[] nd.setUsernameData.username;
                    CULog("setting %d username to ", id1);
                    CULog("username is %s", username1.c_str());
                }
                    break;
                case ND::NetworkData::SET_MAP_NUMBER:
                {
                    mapSelected = nd.setMapNumber.mapNumber;
                    CULog("set map selected to %d", mapSelected);
                }
                    break;
                case ND::NetworkData::TAG_PACKET:
                {
                    auto tagged = world->getPlayer(nd.tagData.taggedId);
                    auto tagger = world->getPlayer(nd.tagData.taggerId);
                    auto self = world->getPlayer(network->getPlayerID().value());
                    tagged->setIsTagged(true);
                    tagged->setTagCooldown(nd.tagData.timestamp);
                    tagger->incScore(globals::TAG_SCORE);
                    SoundController::playSound(SoundController::Type::TAG, tagger->getPosition() - self->getPosition());
                    if (tagged->getCurrElement() == Element::None && !nd.tagData.dropEgg) {
                        auto egg = world->getEgg(tagged->getEggId());
                        egg->setPID(tagger->getID());
                        tagged->setElement(tagged->getPrevElement());
                        tagged->setHoldingEgg(false);
                        egg->setDistanceWalked(0);
                        tagger->setElement(Element::None);
                        tagger->setEggId(egg->getID());
                        tagger->setHoldingEgg(true);
                    }
                }
                    break;
                case ND::NetworkData::POSITION_PACKET:
                    {
                        auto p = world->getPlayer(nd.positionData.playerId);
                        auto newError = (p->getPosition() + p->getPositionError()) - nd.positionData.playerPos;
                        p->setPositionError(newError);
                        p->setPosition(nd.positionData.playerPos);
                        p->setDirection(nd.positionData.angle);
                        p->setLinearVelocity(nd.positionData.playerVelocity);
                    }
                    break;
                case ND::NetworkData::ORB_CAPTURED:
                {
                    auto o = world->getOrb(nd.orbCapData.orbId);
                    o->setCollected(true);
                    auto p = world->getPlayer(nd.orbCapData.playerId);
                    p->setOrbScore(p->getOrbScore() + 1);
                    auto self = world->getPlayer(network->getPlayerID().value());
                    SoundController::playSound(SoundController::Type::ORB, o->getPosition() - self->getPosition());
                    break;
                }
                case ND::NetworkData::SWAP_PACKET:
                {
                    world->getPlayer(nd.swapData.playerId)->setElement(nd.swapData.newElement);
                    auto s = world->getSwapStation(nd.swapData.swapId);
                    s->setLastUsed(clock());
                    s->setActive(false);
                    auto self = world->getPlayer(network->getPlayerID().value());
                    SoundController::playSound(SoundController::Type::SWAP, s->getPosition() - self->getPosition());
                    break;
                }
                case ND::NetworkData::EGG_CAPTURED:
                {
                    auto p = world->getPlayer(nd.eggCapData.playerId);
                    p->setElement(Element::None);
                    p->setEggId(nd.eggCapData.eggId);
                    p->setHoldingEgg(true);
                    auto e = world->getEgg(nd.eggCapData.eggId);
                    e->setCollected(true);
                    e->setPID(nd.eggCapData.playerId);
                    auto self = world->getPlayer(network->getPlayerID().value());
                    SoundController::playSound(SoundController::Type::EGG, e->getPosition() - self->getPosition());

                }
                    break;
                case ND::NetworkData::EGG_HATCHED:
                {
                    auto p = world->getPlayer(nd.eggHatchData.playerId);
                    auto egg = world->getEgg(nd.eggCapData.eggId);
                    p->setElement(p->getPrevElement());
                    p->setHoldingEgg(false);
                    world->getEgg(nd.eggCapData.eggId)->setHatched(true);
                    world->setCurrEggCount(world->getCurrEggCount() - 1);
                    p->incScore(globals::HATCH_SCORE);
                    world->addEggSpawn(egg->getSpawnLoc());
                }
                    break;
                case ND::NetworkData::ORB_RESPAWN:
                {
                    auto orb = world->getOrb(nd.orbRespawnData.orbId);
                    orb->setPosition(nd.orbRespawnData.position);
                    orb->setCollected(false);
                }
                    break;
                case ND::NetworkData::ELEMENT_CHANGE:
                    world->getPlayer(nd.elementChangeData.playerId)->setElement(nd.elementChangeData.newElement);
                    break;
                case ND::NetworkData::PROJECTILE_FIRED:
                {
                    auto projectile = world->getProjectile(nd.projectileFiredData.projectileId);
                    auto pos = nd.projectileFiredData.projectilePos;
                    auto angle = nd.projectileFiredData.projectileAngle;
                    projectile->setActive(true);
                    projectile->setPreyElement(nd.projectileFiredData.preyElement);
                    projectile->setPosition(pos);
                    projectile->getSceneNode()->setVisible(true);
                    projectile->setLinearVelocity(Vec2::forAngle(angle + M_PI / 2) * 25);
                    projectile->setAngle(angle);
                    projectile->getSceneNode()->setAngle(angle + M_PI);
                }
                    break;
                case ND::NetworkData::PROJECTILE_GONE:
                {
                    auto projectile = world->getProjectile(nd.projectileGoneData.projectileId);
                    projectile->setActive(false);
                    projectile->getSceneNode()->setVisible(false);
                    projectile->setLinearVelocity(Vec2(0, 0));
                    projectile->setPosition(Vec2(0, 0));
                }
                    break;
                case ND::NetworkData::EGG_RESPAWN:
                {
                    auto egg = world->getEgg(nd.eggRespawnData.eggId);
                    egg->setPosition(nd.eggRespawnData.position);
                    egg->setCollected(false);
                    egg->setHatched(false);
                    egg->setDistanceWalked(0);
                    egg->setSpawnLoc(nd.eggRespawnData.position);
                    egg->setInitPos(nd.eggRespawnData.position);
                }
                    break;
            }
        });
    }
    //send current player's position
    void sendPosition(){
        if(! getPlayerId().has_value()) return;
        _networkFrame = (_networkFrame + 1) % NETWORK_FRAMERATE;
        if(_networkFrame != 0) return;
        auto p = world->getPlayer(getPlayerId().value());
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::POSITION_PACKET;
        nd.positionData.playerPos = p->getPosition();
        nd.positionData.playerVelocity = p->getLinearVelocity();
        nd.positionData.angle = p->getDirection();
        nd.positionData.playerId = NetworkController::getPlayerId().value();
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    //send when a player captures an orb
    void sendOrbCaptured(int orbId, int playerId){
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::ORB_CAPTURED;
        nd.orbCapData.orbId = orbId;
        nd.orbCapData.playerId = playerId;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    //send when a player swaps colors at a swap station
    void sendPlayerColorSwap(int playerId, Element newElement, int swapId){
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::SWAP_PACKET;
        nd.swapData.newElement = newElement;
        nd.swapData.playerId = playerId;
        nd.swapData.swapId = swapId;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    //send when a player collects an egg
    void sendEggCollected(int playerId, int eggId){
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::EGG_CAPTURED;
        nd.eggCapData.eggId = eggId;
        nd.eggCapData.playerId = playerId;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void sendOrbRespawn(int orbId, Vec2 orbPosition){
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::ORB_RESPAWN;
        nd.orbRespawnData.orbId = orbId;
        nd.orbRespawnData.position = orbPosition;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void sendElementChange(int playerId, Element newElement) {
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::ELEMENT_CHANGE;
        nd.elementChangeData.playerId = playerId;
        nd.elementChangeData.newElement = newElement;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
}

    void sendEggRespawn(int eggId, Vec2 eggPosition){
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::EGG_RESPAWN;
        nd.eggRespawnData.eggId = eggId;
        nd.eggRespawnData.position = eggPosition;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void sendProjectileFired(int projectileId, Vec2 projectilePos, float projectileAngle, Element preyElement) {
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::PROJECTILE_FIRED;
        nd.projectileFiredData.projectileId = projectileId;
        nd.projectileFiredData.projectilePos = projectilePos;
        nd.projectileFiredData.projectileAngle = projectileAngle;
        nd.projectileFiredData.preyElement = preyElement;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void sendEggHatched(int playerId, int eggId) {
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::EGG_HATCHED;
        nd.eggHatchData.eggId = eggId;
        nd.eggHatchData.playerId = playerId;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void sendProjectileGone(int projectileId) {
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::PROJECTILE_GONE;
        nd.projectileGoneData.projectileId = projectileId;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void sendTag(int taggedId, int taggerId, time_t timestamp, bool dropEgg){
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::TAG_PACKET;
        nd.tagData.taggedId = taggedId;
        nd.tagData.taggerId = taggerId;
        nd.tagData.timestamp = timestamp;
        nd.tagData.dropEgg = dropEgg;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void sendSetUsername(int playerId, string username) {
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::SET_USERNAME;
        nd.setUsernameData.playerId = playerId;
        nd.setUsernameData.username_length = username.length();
        nd.setUsernameData.username = username.data();
        //CULog("reached here 1");
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void sendSetMapSelected(int i) {
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::SET_MAP_NUMBER;
        nd.setMapNumber.mapNumber = i;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void ready(){
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::CLIENT_READY;
        nd.readyData.player_id = network->getPlayerID().value();
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void unready(){
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::CLIENT_UNREADY;
        nd.readyData.player_id = network->getPlayerID().value();
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

    void startGame(){
        network->startGame();
        ND::NetworkData nd{};
        nd.packetType = ND::NetworkData::PacketType::HOST_STARTGAME;
        std::vector<uint8_t> bytes;
        ND::toBytes(bytes, nd);
        network->send(bytes);
    }

}
