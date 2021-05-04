
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

    void setReadyCallback(std::function<void(uint8_t, bool)> cb){
        readyCallback = cb;
    }

    void setStartCallback (std::function<void(void)> cb){
        startCallback = cb;
    }

struct LobbyHandler {
    LobbyHandler(){};
    void operator()(NetworkData::Ready & data) const {
        readyCallback(data.player_id, true);
    }
    void operator()(NetworkData::Unready & data) const {
        readyCallback(data.player_id, false);
    }
    void operator()(NetworkData::StartGame & data) const {
        startCallback();
    }
    //generic. do nothing
    template<typename T>
    void operator()(T & d) const {}
};

struct GameHandler {
    GameHandler(){};
    void operator()(NetworkData::Tag & t) const {
        auto tagged = world->getPlayer(t.taggedId);
        auto tagger = world->getPlayer(t.taggerId);
        auto self = world->getPlayer(network->getPlayerID().value());
        tagged->setIsTagged(true);
        tagged->setTagCooldown(t.timestamp);
        tagger->incScore(globals::TAG_SCORE);
        SoundController::playSound(SoundController::Type::TAG, tagger->getPosition() - self->getPosition());
        if (tagged->getCurrElement() == Element::None && !t.dropEgg) {
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
    void operator()(NetworkData::OrbRespawn & data) const {
        auto orb = world->getOrb(data.orbId);
        orb->setPosition(data.position);
        orb->setCollected(false);
    }
    void operator()(NetworkData::EggRespawn & data) const {
        auto egg = world->getEgg(data.eggId);
        egg->setPosition(data.position);
        egg->setCollected(false);
        egg->setHatched(false);
        egg->setDistanceWalked(0);
        egg->setInitPos(Vec2(10,10));
    }
    void operator()(NetworkData::EggCapture & data) const {
        auto p = world->getPlayer(data.playerId);
        p->setElement(Element::None);
        p->setEggId(data.eggId);
        p->setHoldingEgg(true);
        auto e = world->getEgg(data.eggId);
        e->setCollected(true);
        e->setPID(data.playerId);
        auto self = world->getPlayer(network->getPlayerID().value());
        SoundController::playSound(SoundController::Type::EGG, e->getPosition() - self->getPosition());
    }
    void operator()(NetworkData::EggHatch & data) const {
        auto p = world->getPlayer(data.playerId);
        p->setElement(p->getPrevElement());
        p->setHoldingEgg(false);
        world->getEgg(data.eggId)->setHatched(true);
        world->setCurrEggCount(world->getCurrEggCount() - 1);
    }
    void operator()(NetworkData::OrbCapture & data) const {
        auto o = world->getOrb(data.orbId);
        o->setCollected(true);
        auto p = world->getPlayer(data.playerId);
        p->setOrbScore(p->getOrbScore() + 1);
        auto self = world->getPlayer(network->getPlayerID().value());
        SoundController::playSound(SoundController::Type::ORB, o->getPosition() - self->getPosition());
    }
    void operator()(NetworkData::Swap & data) const {
        world->getPlayer(data.playerId)->setElement(data.newElement);
        auto s = world->getSwapStation(data.swapId);
        s->setLastUsed(clock());
        s->setActive(false);
        auto self = world->getPlayer(network->getPlayerID().value());
        SoundController::playSound(SoundController::Type::SWAP, s->getPosition() - self->getPosition());
    }
    void operator()(NetworkData::Position & data) const {
        auto p = world->getPlayer(data.playerId);
        auto newError = (p->getPosition() + p->getPositionError()) - data.playerPos;
        p->setPositionError(newError);
        p->setPosition(data.playerPos);
        p->setDirection(data.angle);
        p->setLinearVelocity(data.playerVelocity);
    }
    void operator()(NetworkData::SetUsername & data) const {
        int id1 = data.playerId;
        string username1 = data.username;
    }
    void operator()(NetworkData::SetMap & data) const {
        mapSelected = data.mapNumber;
    }
    void operator()(NetworkData::ElementChange & data) const {
        world->getPlayer(data.playerId)->setElement(data.newElement);
    }
    void operator()(NetworkData::ProjectileFired & data) const {
        auto projectile = world->getProjectile(data.projectileId);
        auto pos = data.projectilePos;
        auto angle = data.projectileAngle;
        projectile->setActive(true);
        projectile->setPreyElement(data.preyElement);
        projectile->setPosition(pos);
        projectile->getSceneNode()->setVisible(true);
        projectile->setLinearVelocity(Vec2::forAngle(angle + M_PI / 2) * 25);
        projectile->setAngle(angle);
        projectile->getSceneNode()->setAngle(angle + M_PI);
    }
    void operator()(NetworkData::ProjectileGone & data) const {
        auto projectile = world->getProjectile(data.projectileId);
        projectile->setActive(false);
        projectile->getSceneNode()->setVisible(false);
        projectile->setLinearVelocity(Vec2(0, 0));
        projectile->setPosition(Vec2(0, 0));
    }
    //generic. do nothing
    template<typename T>
    void operator()(T & d) const {}
};

    void step() {
        if(network == nullptr) return;
        network->receive([&](const std::vector<uint8_t> msg) {
            NetworkData nd = NetworkData::fromBytes(msg);
            LobbyHandler handler;
            std::visit(handler, nd.data);
        });
    }

    void update(float timestep){
        network->receive([&](const std::vector<uint8_t> msg) {
            NetworkData nd = NetworkData::fromBytes(msg);
            GameHandler handler;
            std::visit(handler, nd.data);
        });
    }
    
    void send(NetworkData d){
        network->send(d.toBytes());
    }

    //send current player's position
    void sendPosition(){
        if(! getPlayerId().has_value()) return;
        _networkFrame = (_networkFrame + 1) % NETWORK_FRAMERATE;
        if(_networkFrame != 0) return;
        auto p = world->getPlayer(getPlayerId().value());
        NetworkData::Position pos;
        pos.playerPos = p->getPosition();
        pos.playerVelocity = p->getLinearVelocity();
        pos.angle = p->getDirection();
        pos.playerId = NetworkController::getPlayerId().value();
        send(NetworkData(pos));
    }

    //send when a player captures an orb
    void sendOrbCaptured(int orbId, int playerId){
        NetworkData::OrbCapture o;
        o.orbId = orbId;
        o.playerId = playerId;
        send(NetworkData(o));
    }

    //send when a player swaps colors at a swap station
    void sendPlayerColorSwap(int playerId, Element newElement, int swapId){
        NetworkData::Swap s;
        s.newElement = newElement;
        s.playerId = playerId;
        s.swapId = swapId;
        send(NetworkData(s));
    }

    //send when a player collects an egg
    void sendEggCollected(int playerId, int eggId){
        NetworkData::EggCapture e;
        e.eggId = eggId;
        e.playerId = playerId;
        send(NetworkData(e));
    }

    void sendOrbRespawn(int orbId, Vec2 orbPosition){
        NetworkData::OrbRespawn o;
        o.orbId = orbId;
        o.position = orbPosition;
        send(NetworkData(o));
    }

    void sendElementChange(int playerId, Element newElement) {
        NetworkData::ElementChange e;
        e.playerId = playerId;
        e.newElement = newElement;
        send(NetworkData(e));
    }

    void sendEggRespawn(int eggId, Vec2 eggPosition){
        NetworkData::EggRespawn e;
        e.eggId = eggId;
        e.position = eggPosition;
        send(NetworkData(e));
    }

    void sendProjectileFired(int projectileId, Vec2 projectilePos, float projectileAngle, Element preyElement) {
        NetworkData::ProjectileFired p;
        p.projectileId = projectileId;
        p.projectilePos = projectilePos;
        p.projectileAngle = projectileAngle;
        p.preyElement = preyElement;
        send(NetworkData(p));
    }

    void sendEggHatched(int playerId, int eggId) {
        NetworkData::EggHatch e;
        e.eggId = eggId;
        e.playerId = playerId;
        send(NetworkData(e));
    }

    void sendProjectileGone(int projectileId) {
        NetworkData::ProjectileGone p;
        p.projectileId = projectileId;
        send(NetworkData(p));
    }

    void sendTag(int taggedId, int taggerId, time_t timestamp, bool dropEgg){
        NetworkData::Tag t;
        t.taggedId = taggedId;
        t.taggerId = taggerId;
        t.timestamp = timestamp;
        t.dropEgg = dropEgg;
        send(NetworkData(t));
    }

    void sendSetUsername(int playerId, string username) {
        NetworkData::SetUsername data;
        data.playerId = playerId;
        data.username = username;
        send(NetworkData(data));
    }

    void sendSetMapSelected(int i) {
        NetworkData::SetMap data;
        data.mapNumber = i;
        send(NetworkData(data));
    }

    void ready(){
        NetworkData::Ready d;
        d.player_id = network->getPlayerID().value();
        send(NetworkData(d));
    }

    void unready(){
        NetworkData::Unready d;
        d.player_id = network->getPlayerID().value();
        send(NetworkData(d));
    }

    void startGame(){
        network->startGame();
        NetworkData::StartGame s;
        send(NetworkData(s));
    }

}
