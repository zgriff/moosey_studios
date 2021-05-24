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
        std::shared_ptr<World> world;
        std::string roomId;
        //Username would need to go from LoadingScene to GameScene so more convenient as a global variable
        std::string username = "";
        //Networked usernames indexed by playerId
        array<std::string, 8> usernames = {"Player 1", "Player 2", "Player 3" , "Player 4" , 
            "Player 5" , "Player 6" , "Player 7" , "Player 8" };
        
        bool disconnected = false;
        std::string disconnectedMessage = "";

        std::unordered_map<int,std::tuple<int,int,Element>> customizations;
    
        int _networkFrame;
        int mapSelected = 1;
        std::function<void(uint8_t, bool)> readyCallback;
        std::function<void(void)> startCallback;
        time_t startTimestamp;
    }

    /** IP of the NAT punchthrough server */
//    constexpr auto SERVER_ADDRESS = "35.208.113.51"; //josh's;
    //constexpr auto SERVER_ADDRESS = "34.74.68.73"; //michael's
    constexpr auto SERVER_ADDRESS = "34.75.79.230"; //ours
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

    void destroyConn() {
        network = nullptr;
        roomId = "";
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
        return roomId;
    }

    uint8_t getNumPlayers() {
        return network->getNumPlayers();
    }

    bool isPlayerActive(uint8_t playerID) {
        return network->isPlayerActive(playerID);
    }

    std::string getUsername() {
        if (username == "") {
            return "Player";
        }
        return NetworkController::username;
    }

    std::string getUsername(int playerId) {
        return usernames[playerId];
    }

    void setUsername(std::string name) {
        username = name;
    }

    void setUsername(std::string name, int playerId) {
        usernames[playerId] = name;
    }

    bool getDisconnected() { return disconnected; }

    void setDisconnected(bool b) { disconnected = b; }

    std::string getDisconnectedMessage() { return disconnectedMessage; }

    std::unordered_map<int,std::tuple<int,int,Element>> getCustomizations() {
        return customizations;
    }

    std::tuple<int,int,Element> getCustomization(int playerId) {
        return customizations[playerId];
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

    time_t getStartTimestamp() {
        return startTimestamp;
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
        startTimestamp = data.timestamp;
        startCallback();
    }
    void operator()(NetworkData::SetMap & data) const {
        mapSelected = data.mapNumber;
    }
    void operator()(NetworkData::SetUsername& data) const {
        int id1 = data.playerId;
        string username1 = data.username;
        char nullChar = 0;
        for (int i = 0; i < id1; i++) {
            if (username1 == usernames[i]) {
                char lastChar = username1.back();
                if (isdigit(lastChar)) {
                    if (lastChar == '9') {
                        username1 = username1.substr(0, username1.size() - 1) + " 2";
                    }
                    else {
                        int charToInt = lastChar - '0' + 1;
                        username1 = username1.substr(0, username1.size() - 1) + " " + std::to_string(charToInt);
                    }
                }
                else {
                    username1 = username1 + " 2";
                }
                sendSetUsername(id1, username1);
            }
        }
        if (network->getPlayerID().has_value()) {
            if (network->getPlayerID().value() == id1) {
                username = username1;
            }
        }
        usernames[id1] = username1;
    }
    void operator()(NetworkData::SetCustomization& data) const {
        int id1 = data.playerId;
        customizations[id1] = make_tuple(data.skin, data.hat, data.element);
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
        auto players = world->getPlayers();
        for (auto it = players.begin(); it != players.end(); it++) {
            if (Vec2((*it)->getPosition() - tagged->getPosition()).length() < globals::TAG_ASSIST_DIST &&
                (*it)->getPreyElement() == tagged->getCurrElement() &&
                (*it)->getID() != tagger->getID()) {
                (*it)->incScore(globals::TAG_ASSIST_SCORE);
            }
        }
        tagged->setIsTagged(true);
        tagged->setTimeLastTagged(t.timestamp);
        tagged->animateTagged();
        tagger->incScore(globals::TAG_SCORE);
        tagger->animateTag();
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
        egg->setSpawnLoc(data.position);
        egg->setInitPos(data.position);
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
        auto egg = world->getEgg(data.eggId);
        egg->setHatched(true);
        world->setCurrEggCount(world->getCurrEggCount() - 1);
        p->incScore(globals::HATCH_SCORE);
        world->addEggSpawn(egg->getSpawnLoc());
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
        s->setLastUsed(time(NULL));
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
    void operator()(NetworkData::LeftGame& data) const {
        auto player = world->getPlayer(data.playerId);
        player->setActive(false);
        player->getSceneNode()->setVisible(false);
        player->setLinearVelocity(Vec2(0, 0));
        player->setPosition(Vec2(0, 0));
        disconnected = true;
        disconnectedMessage = usernames[data.playerId] + " has disconnected.";
        //player->deactivatePhysics(world); 
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

    void sendSetCustomization(int playerId, int skin, int hat, Element element) {
        NetworkData::SetCustomization data;
        customizations[playerId] =  make_tuple(skin, hat, element);
        data.playerId = playerId;
        data.skin = skin;
        data.hat = hat;
        data.element = element;
        send(NetworkData(data));
    }

    void sendSetMapSelected(int i) {
        NetworkData::SetMap data;
        data.mapNumber = i;
        CULog("sending set map to %d", i);
        send(NetworkData(data));
    }

    void sendLeftGame(int i) {
        NetworkData::LeftGame data;
        data.playerId = i;
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
        startTimestamp = time(NULL);
        s.timestamp = startTimestamp;
        send(NetworkData(s));
    }

}
