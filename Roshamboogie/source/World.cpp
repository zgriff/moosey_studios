//
//  World.cpp
//  Roshamboogie
//
//  Created by Joshua Kaplan on 3/25/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "World.h"
#include "Globals.h"
#include "MapConstants.h"

/** The initial player position */
float PLAYER_POS[] = {24,  4};

World::World(void) : Asset(),
_root(nullptr),
_physicsWorld(nullptr),
_worldNode(nullptr),
_debugNode(nullptr)
{
    _bounds.size.set(1.0f, 1.0f);
}

/**
* Destroys this level, releasing all resources.
*/
World::~World(void) {
    unload();
    clearRootNode();
}

#pragma mark -
#pragma mark Drawing Methods

//void World::setDrawScale(float value) {
//    if (_rocket != nullptr) {
//        _rocket->setDrawScale(value);
//    }
//}

void World::setRootNode(const std::shared_ptr<scene2::SceneNode>& root, float scale) {
    if (_root !=  nullptr) {
        clearRootNode();
    }
    
    _root = root;
    _scale = scale;
    CULog("world pos x: %f y: %f", getPhysicsWorld()->getBounds().getMaxX(), getPhysicsWorld()->getBounds().getMaxY());
    
    CULog("scale x: %f ", _scale);
    
    // Create, but transfer ownership to root
    _worldNode = scene2::SceneNode::alloc();
    _worldNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldNode->setPosition(Vec2::ZERO);
    
    _debugNode = scene2::SceneNode::alloc();
    _debugNode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugNode->setPosition(Vec2::ZERO);
    
    _root->addChild(_worldNode,0);
    _root->addChild(_debugNode,1);

    
    // Add the individual elements
    std::shared_ptr<scene2::PolygonNode> poly;
    std::shared_ptr<scene2::WireNode> draw;
    
    auto playerTexture = _assets->get<Texture>("player");
    auto orbTexture = _assets->get<Texture>("orb");
    auto swapStTexture = _assets->get<Texture>("swapstation");
    auto eggTexture = _assets->get<Texture>("egg");
//
//    for(auto it = _walls.begin(); it != _walls.end(); ++it) {
//        std::shared_ptr<physics2::PolygonObstacle> wall = *it;
//        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("earth"));
//        addObstacle(wall,sprite,1);   // PUT SAME TEXTURES IN SAME LAYER!!!
//    }
    
    for(auto it = _eggs.begin(); it != _eggs.end(); ++it) {
        std::shared_ptr<Egg> egg = *it;
        _physicsWorld->addObstacle(egg);
        egg->setTextures(eggTexture);
//        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("egg"));
        addObstacle(egg,egg->getSceneNode(),2);   // PUT SAME TEXTURES IN SAME LAYER!!!
    }
    
    int counter = 0;
    for(auto it = _orbs.begin(); it != _orbs.end(); ++it) {
        std::shared_ptr<Orb> orb = *it;
        orb->setID(counter);
//        orb->setTextures(_assets->get<Texture>("swaporb"));
        CULog("orb pos x: %f y: %f", orb->getPosition().x, orb->getPosition().y);
        _physicsWorld->addObstacle(orb);
        orb->setTextures(orbTexture);
//        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("swaporb"));
        addObstacle(orb,orb->getSceneNode(),1);
    }
    
    for(auto it = _swapStations.begin(); it != _swapStations.end(); ++it) {
        std::shared_ptr<SwapStation> station = *it;
//        station->setTextures(_assets->get<Texture>("swapstation"));
        _physicsWorld->addObstacle(station);
        station->setTextures(swapStTexture);
//        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("swapstation"));
        addObstacle(station,station->getSceneNode(),1);
    }
    
    Vec2 playerPos = ((Vec2)PLAYER_POS);
    Size playerSize(1, 2);
    for(int i = 0; i < _numPlayers; ++i){ //TODO: Change to _numPlayers once that's locked in
        auto player = Player::alloc(playerPos, playerSize, Element::Water);
        _physicsWorld->addObstacle(player);
        player->setTextures(playerTexture);
        addObstacle(player, player->getSceneNode(), 2);
        player->setID(i);
        player->setDrawScale(_scale);
        player->setDebugColor(Color4::YELLOW);
        player->setDebugScene(_debugNode);
        player->allocUsernameNode(_assets->get<Font>("username"));
        _players.push_back(player);
    }
    
}

void World::clearRootNode() {
    if (_root == nullptr) {
        return;
    }
    _worldNode->removeFromParent();
    _worldNode->removeAllChildren();
    _worldNode = nullptr;
  
    _debugNode->removeFromParent();
    _debugNode->removeAllChildren();
    _debugNode = nullptr;

    _root = nullptr;
}

void World::showDebug(bool flag) {
    if (_debugNode != nullptr) {
        _debugNode->setVisible(flag);
    }
}

#pragma mark -
#pragma mark Asset Loading
/**
 * Loads this game level from the source file
 *
 * This load method should NEVER access the AssetManager.  Assets are loaded in
 * parallel, not in sequence.  If an asset (like a game level) has references to
 * other assets, then these should be connected later, during scene initialization.
 *
 * @return true if successfully loaded the asset from a file
 */
bool World::preload(const std::string& file) {
    std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(file);
    return preload(reader->readJson());
}

/**
 * Loads this game level from the source file
 *
 * This load method should NEVER access the AssetManager.  Assets are loaded in
 * parallel, not in sequence.  If an asset (like a game level) has references to
 * other assets, then these should be connected later, during scene initialization.
 *
 * @return true if successfully loaded the asset from a file
 */
bool World::preload(const std::shared_ptr<cugl::JsonValue>& json) {
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
        return false;
    }
    
    float w = json->get(WIDTH_FIELD)->asFloat();
    float h = json->get(HEIGHT_FIELD)->asFloat();
    
    _bounds.size.set(w * globals::TILE_TO_BOX2D , h* globals::TILE_TO_BOX2D);
    
    _sceneSize = Vec2(w*globals::TILE_TO_SCENE, h*globals::TILE_TO_SCENE);
    
    _physicsWorld = physics2::ObstacleWorld::alloc(getBounds(),Vec2::ZERO);
    
    auto gameObjects = json->get(GAME_OBJECTS_FIELD);
    if (gameObjects != nullptr) {
        int gsize = (int)gameObjects->size();
        for(int ii = 0; ii < gsize; ii++) {
            loadGameObject(gameObjects->get(ii));
        }
    } else {
        CUAssertLog(false, "Failed to load game objects");
        return false;
    }
    
//    auto walls = json->get(WALLS_FIELD);
//    if (walls != nullptr) {
//        int wsize = (int)walls->size();
//        for(int ii = 0; ii < wsize; ii++) {
//            loadWall(walls->get(ii));
//        }
//    } else {
//        CUAssertLog(false, "Failed to load walls");
//        return false;
//    }
    
    
//    auto tiles = json->get(TILES_FIELD);
//    if (tiles != nullptr) {
//        int tsize = (int)tiles->size();
//        for(int ii = 0; ii < tsize; ii++) {
//            loadBackground(tiles->get(ii));
//        }
//    } else {
//        CUAssertLog(false, "Failed to load tiles");
//        return false;
//    }
//
//
//    auto decorations = json->get(DECORATIONS_FIELD);
//    if (decorations != nullptr) {
//        int dsize = (int)decorations->size();
//        for(int ii = 0; ii < dsize; ii++) {
//            loadDecorations(decorations->get(ii));
//        }
//    } else {
//        CUAssertLog(false, "Failed to load decorations");
//        return false;
//    }
    
    return true;
}


//Used to unload objects, likely will not need for now.
void World::unload()  {
    CULog("Unloading world");
    for (auto it = _players.begin(); it != _players.end(); ++it)  {
        if (_physicsWorld != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _players.clear();
    
    for (auto it = _swapStations.begin(); it != _swapStations.end(); ++it)  {
        if (_physicsWorld != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _swapStations.clear();
    
    for (auto it = _eggs.begin(); it != _eggs.end(); ++it)  {
        if (_physicsWorld != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _eggs.clear();
    
    for (auto it = _orbs.begin(); it != _orbs.end(); ++it)  {
        if (_physicsWorld != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _orbs.clear();
    
    for (auto it = _walls.begin(); it != _walls.end(); ++it)  {
        if (_physicsWorld != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _walls.clear();
    
    if (_physicsWorld !=  nullptr) {
        _physicsWorld->clear();
        _physicsWorld = nullptr;
    }
    
}

GameObjectType World::getObjectType(std::string obj) {
    if (obj == SWAP_STATION) {
        return GameObjectType::Station;
    } else if (obj == EGG_SPAWN) {
        return GameObjectType::EggSpawn;
    }  else if (obj == ORB_SPAWN) {
        return GameObjectType::OrbSpawn;
    } else {
        return GameObjectType::PlayerSpawn;
    }
}

#pragma mark -
#pragma mark Object Loading

bool World::loadWall(const std::shared_ptr<JsonValue> &json) {
    bool success = true;
    CULog("loading wall");
//    std::shared_ptr<Texture> image = _assets->get<Texture>("earth");
//    std::shared_ptr<scene2::PolygonNode> sprite;
//    std::shared_ptr<scene2::WireNode> draw;
//    std::string wname = "wall";
//    int polysize = (int)json->size();
    
    
    
//    for (int ii = 0; ii<; ii++) {
//        std::shared_ptr<physics2::PolygonObstacle> wallobj;
//
//        Poly2 wall(WALL[ii],WALL_VERTS);
//        // Call this on a polygon to get a solid shape
//        SimpleTriangulator triangulator;
//        triangulator.set(wall);
//        triangulator.calculate();
//        wall.setIndices(triangulator.getTriangulation());
//        wall.setGeometry(Geometry::SOLID);
//
//        wallobj = physics2::PolygonObstacle::alloc(wall);
//        wallobj->setDebugColor(Color4::WHITE);
//        // You cannot add constant "".  Must stringify
//        wallobj->setName(std::string("wall")+cugl::strtool::to_string(ii));
//        wallobj->setName(wname);
//
//        // Set the physics attributes
//        wallobj->setBodyType(b2_staticBody);
//        wallobj->setDensity(BASIC_DENSITY);
//        wallobj->setFriction(BASIC_FRICTION);
//        wallobj->setRestitution(BASIC_RESTITUTION);
//
//        wall *= _scale;
//        sprite = scene2::PolygonNode::allocWithTexture(image,wall);
//        addObstacle(wallobj,sprite,2);
//    }
    
    return success;
}

bool World::loadStation(const std::shared_ptr<JsonValue> &json) {
    float xCoord = json->getFloat(X_FIELD) * globals::TILE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::TILE_TO_BOX2D;
    
    // **** NEED TO CHANGE SIZE, CANNOT ACCESS _ASSETS IN LOADS
//    auto swapStTexture = _assets->get<Texture>("swapstation");
    Vec2 swapStPos = Vec2(xCoord,yCoord);
//    Size swapStSize(swapStTexture->getSize() / _scale);
    Size swapStSize(2,2);
    auto swapStation = SwapStation::alloc(swapStPos, swapStSize);
    swapStation->setDrawScale(_scale);
    swapStation->setActive(true);
    swapStation->setDebugColor(Color4::YELLOW);
    swapStation->setDebugScene(_debugNode);
    swapStation->setID(0);
    _swapStations.push_back(swapStation);
    return true;
}

bool World::loadEgg(const std::shared_ptr<JsonValue> &json){
    float xCoord = json->getFloat(X_FIELD) * globals::TILE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::TILE_TO_BOX2D;
    
    // **** NEED TO CHANGE SIZE, CANNOT ACCESS _ASSETS IN LOADS
    
//    auto eggTexture = _assets->get<Texture>("target");
    Vec2 eggPos = Vec2(xCoord,yCoord);
//    Size eggSize(eggTexture->getSize() / _scale);
    Size eggSize(1, 2);
    auto egg = Egg::alloc(eggPos, eggSize);
//    egg->setTextures(eggTexture);
    egg->setDrawScale(_scale);
    egg->setActive(true);
    egg->setDebugColor(Color4::YELLOW);
    egg->setDebugScene(_debugNode);
    egg->setID(0);
    _eggs.push_back(egg);
    return true;
}


bool World::loadOrb(const std::shared_ptr<JsonValue> &json){
    float xCoord = json->getFloat(X_FIELD) * globals::TILE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::TILE_TO_BOX2D;
    
    // **** NEED TO CHANGE SIZE, CANNOT ACCESS _ASSETS IN LOADS
    
//    auto eggTexture = _assets->get<Texture>("target");
    Vec2 orbPos = Vec2(xCoord,yCoord);
//    Size orbSize(orbTexture->getSize() / _scale);
    Size orbSize(1, 2);
    CULog("orbPos: %f   y %f",orbPos.x,orbPos.y);
    auto orb = Orb::alloc(orbPos);
    orb->setDrawScale(_scale);
    orb->setActive(true);
    orb->setDebugColor(Color4::YELLOW);
    orb->setDebugScene(_debugNode);
    orb->setID(0);
    _orbs.push_back(orb);
    return true;
}

//bool World::loadPlayer(const int i, Vec2 loc){
//    Size playerSize(1, 2);
//    auto player = Player::alloc(loc, playerSize, Element::Water);
//    _physicsWorld->addObstacle(player);
//    player->setID(i);
//    player->setDrawScale(_scale);
//    player->setDebugColor(Color4::YELLOW);
//    player->setDebugScene(_debugNode);
//    player->allocUsernameNode(_assets->get<Font>("username"));
//    _players.push_back(player);
//    _worldNode->addChild(player->getSceneNode());
//    return true;
//}

//Only get spawn locations from json. wait to load players until numplayers
bool World::loadPlayerSpawn(const std::shared_ptr<JsonValue> &json) {
    float xCoord = json->getFloat(X_FIELD) * globals::TILE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::TILE_TO_BOX2D;
    
    Vec2 spawnPos = Vec2(xCoord,yCoord);
    
    _playerSpawns.push_back(spawnPos);
    return true;
}

//GameObjects are an array in json, need to sort loading by type
bool World::loadGameObject(const std::shared_ptr<JsonValue>& json) {
    bool success = false;
    auto gameObject = json->getString(TYPE_FIELD);
    GameObjectType objectType = getObjectType(gameObject);
    switch (objectType) {
        case GameObjectType::Station:
            success = loadStation(json);
            break;
            
        case GameObjectType::EggSpawn:
            success = loadEgg(json);
            break;
            
        case GameObjectType::OrbSpawn:
            CULog("spawning orb");
            success = loadOrb(json);
            break;
            
        case GameObjectType::PlayerSpawn:
            success = loadPlayerSpawn(json);
            break;
            
        default:
            return false;
            break;
    }
    
    return success;
}


/** Used to add object to actual game scene. Commented out physics world bc have to add before setting textures.
 Likely need to fix that at some point */
void World::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
    const std::shared_ptr<cugl::scene2::SceneNode>& node,
    int zOrder) {
//    _physicsWorld->addObstacle(obj);
    obj->setDebugScene(_debugNode);

    // Position the scene graph node (enough for static objects)
//    node->setPosition(obj->getPosition()*_scale);
    _worldNode->addChild(node,zOrder);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_scale);
            weak->setAngle(obs->getAngle());
        });
    }
}



//void World::reset(){
//    _scale.set(_root->getContentSize().width/_bounds.size.width,
//             _root->getContentSize().height/_bounds.size.height);
//    _physicsWorld->clear();
//    _worldNode->removeAllChildren();
//    
//    auto playerTexture = _assets->get<Texture>("player");
//    auto orbTexture = _assets->get<Texture>("photon");
//    auto swapStTexture = _assets->get<Texture>("swapstation");
//    auto eggTexture = _assets->get<Texture>("target");
//
//    Vec2 playerPos = ((Vec2)PLAYER_POS);
//    Size playerSize(1, 2);
//    for(int i = 0; i < _numPlayers; ++i){ //TODO: Change to _numPlayers once that's locked in
//        auto player = Player::alloc(playerPos, playerSize, Element::Water);
//        _physicsWorld->addObstacle(player);
//        player->setTextures(playerTexture);
//        player->setID(i);
//        player->setDrawScale(_scale);
//        player->setDebugColor(Color4::YELLOW);
//        player->setDebugScene(_debugNode);
//        player->allocUsernameNode(_assets->get<Font>("username"));
//        _players.push_back(player);
//        _worldNode->addChild(player->getSceneNode());
//    }
//    
//    //creating the three orbs
//    auto fireOrb = Orb::alloc(Vec2(4,4), Element::Fire);
//    _physicsWorld->addObstacle(fireOrb);
//    fireOrb->setTextures(orbTexture);
//    fireOrb->setDrawScale(_scale);
//    fireOrb->setDebugColor(Color4::YELLOW);
//    fireOrb->setDebugScene(_debugNode);
//    fireOrb->setID(0);
//    _orbs.push_back(fireOrb);
//
//    auto waterOrb = Orb::alloc(Vec2(20,8), Element::Water);
//    _physicsWorld->addObstacle(waterOrb);
//    waterOrb->setTextures(orbTexture);
//    waterOrb->setDrawScale(_scale);
//    waterOrb->setDebugColor(Color4::YELLOW);
//    waterOrb->setDebugScene(_debugNode);
//    waterOrb->setID(1);
//    _orbs.push_back(waterOrb);
//    
//    auto grassOrb = Orb::alloc(Vec2(10,12), Element::Grass);
//    _physicsWorld->addObstacle(grassOrb);
//    grassOrb->setTextures(orbTexture);
//    grassOrb->setDrawScale(_scale);
//    grassOrb->setDebugColor(Color4::YELLOW);
//    grassOrb->setDebugScene(_debugNode);
//    grassOrb->setID(2);
//    _orbs.push_back(grassOrb);
//    
//    
//    Vec2 swapStPos = Vec2(8,8);
//    Size swapStSize(swapStTexture->getSize() / _scale);
//    auto swapStation = SwapStation::alloc(swapStPos, swapStSize);
//    _physicsWorld->addObstacle(swapStation);
//    swapStation->setTextures(swapStTexture);
//    swapStation->setDrawScale(_scale);
//    swapStation->setActive(true);
//    swapStation->setDebugColor(Color4::YELLOW);
//    swapStation->setDebugScene(_debugNode);
//    swapStation->setID(0);
//    _swapStations.push_back(swapStation);
//    
//    Vec2 eggPos = Vec2(14,14);
//    Size eggSize(eggTexture->getSize() / _scale);
//    auto egg = Egg::alloc(eggPos, eggSize);
//    _physicsWorld->addObstacle(egg);
//    egg->setTextures(eggTexture);
//    egg->setDrawScale(_scale);
//    egg->setActive(true);
//    egg->setDebugColor(Color4::YELLOW);
//    egg->setDebugScene(_debugNode);
//    egg->setID(0);
//    _eggs.push_back(egg);
//    
//    _worldNode->addChild(fireOrb->getSceneNode());
//    _worldNode->addChild(waterOrb->getSceneNode());
//    _worldNode->addChild(grassOrb->getSceneNode());
//    _worldNode->addChild(swapStation->getSceneNode());
//    _worldNode->addChild(egg->getSceneNode());
//}




//
//void World::dispose(){
//    _players.clear();
//    _eggs.clear();
//    _orbs.clear();
//    _swapStations.clear();
//    _worldNode = nullptr;
//    _debugNode = nullptr;
//    _physicsWorld = nullptr;
//    _assets = nullptr;
//}

//bool World::init(int width, int height){
//    Rect rect(0,0,width,height);
//    _physicsWorld = physics2::ObstacleWorld::alloc(rect,Vec2::ZERO);
//    _worldNode = scene2::SceneNode::alloc();
//    _worldNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
//
//    return true;
//}
//    auto players = json->get(PLAYER_SPAWN_FIELD);
//    if(players != nullptr) {
//        int psize = (int)players->size();
//        for (int ii = 0; ii < psize; ii++) {
//            loadPlayer(players->get(ii));
//        }
//    } else {
//        CUAssertLog(false, "failed to load players");
//        return false;
//    }
    
//
//    auto orbs = json->get(ORB_SPAWN_FIELD);
//    if (orbs != nullptr) {
//        int osize = (int)orbs->size();
//        for(int ii = 0; ii < osize; ii++) {
//            loadOrb(orbs->get(ii));
//        }
//    } else {
//        CUAssertLog(false, "Failed to load walls");
//        return false;
//    }
//
    
//    auto stations = json->get(SWAP_STATIONS_FIELD);
//    if (stations != nullptr) {
//        int ssize = (int)stations->size();
//        for(int ii = 0; ii < ssize; ii++) {
//            loadStation(stations->get(ii));
//        }
//    } else {
//        CUAssertLog(false, "Failed to load walls");
//        return false;
//    }
//
//    auto eggs = json->get(EGG_SPAWN_FIELD);
//    if (eggs != nullptr) {
//        int esize = (int)eggs->size();
//        for(int ii = 0; ii < esize; ii++) {
//            loadStation(eggs->get(ii));
//        }
//    } else {
//        CUAssertLog(false, "Failed to load walls");
//        return false;
//    }
