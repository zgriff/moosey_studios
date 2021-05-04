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

//Function to attach all elements in world scene to root node of game scene
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
    auto boosterTexture = _assets->get<Texture>("booster");
    auto projectileTexture = _assets->get<Texture>("projectile");
//
    //Currently adding elements in back to front order
    //TODO create ordered nodes so if we need to change the order of layering
    // we can just alter z coord -- will likely need custom draw method
    for(auto it = _bgTiles.begin(); it!= _bgTiles.end();  ++it) {
        std::string name = get<0>(*it);
        Vec2 pos = get<1>(*it);
        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(name));
        sprite->setPosition(pos*_scale);
        _worldNode->addChild(sprite,0);
    }
    
    for(auto it = _walls.begin(); it != _walls.end(); ++it) {
        std::shared_ptr<physics2::PolygonObstacle> wall = *it;
        _physicsWorld->addObstacle(wall);
        wall->setDebugScene(_debugNode);
        wall->setName("wall");
    }
    
    _currEggCount = 0;
    _totalEggCount = 0;
    for(auto it = _initEggLocs.begin(); it != _initEggLocs.end(); ++it) {
        Size eggSize(1, 2);
        std::shared_ptr<Egg> egg = Egg::alloc(*it, eggSize);
        _physicsWorld->addObstacle(egg);
        egg->setTextures(eggTexture);
        egg->setDrawScale(_scale);
        egg->setDebugColor(Color4::YELLOW);
        egg->setDebugScene(_debugNode);
        egg->setID(_totalEggCount);
        //setting collected and hatched to true since should not be in map in the beginning
        egg->setCollected(true);
        egg->setHatched(true);
        egg->setDistanceWalked(0);
        egg->setInitPos(egg->getPosition());
        _totalEggCount = _totalEggCount + 1;
        _worldNode->addChild(egg->getSceneNode(),1);
        _eggs.push_back(egg);
    }
    
    _currOrbCount = 0;
    _initOrbCount = 0;
    for(auto it = _initOrbLocs.begin(); it != _initOrbLocs.end(); ++it) {
        CULog("orb");
        std::shared_ptr<Orb> orb = Orb::alloc(*it);
        _physicsWorld->addObstacle(orb);
        orb->setTextures(orbTexture);
        orb->setDrawScale(_scale);
        orb->setCollected(false);
        orb->setActive(true);
        orb->setDebugColor(Color4::YELLOW);
        orb->setDebugScene(_debugNode);
        orb->setID(_currOrbCount);
        _currOrbCount = _currOrbCount + 1;
        _initOrbCount = _initOrbCount + 1;
        _worldNode->addChild(orb->getSceneNode(),1);
        _orbs.push_back(orb);
    }
    
    for(auto it = _swapStations.begin(); it != _swapStations.end(); ++it) {
        std::shared_ptr<SwapStation> station = *it;
//        station->setDimension(swapStTexture->getSize() / _scale);
        _physicsWorld->addObstacle(station);
        station->setDrawScale(_scale);
        station->setActive(true);
        station->setLastUsed(time(NULL));
        station->setDebugColor(Color4::YELLOW);
        station->setDebugScene(_debugNode);
        station->setID(0);
        station->setTextures(swapStTexture);
        _worldNode->addChild(station->getSceneNode(),1);
    }

    for (auto it = _boosters.begin(); it != _boosters.end(); ++it) {
        std::shared_ptr<Booster> booster = *it;
        _physicsWorld->addObstacle(booster);
        booster->setDrawScale(_scale);
        booster->setActive(true);
        booster->setDebugColor(Color4::YELLOW);
        booster->setDebugScene(_debugNode);
        booster->setID(0);
        booster->setTextures(boosterTexture);
        _worldNode->addChild(booster->getSceneNode(), 1);
    }
    

    //this is for if we want to move projectile to world, but theres bug with this, with this being null for some reason
    for (int i = 0; i < _numPlayers; ++i) {
        Size projSize(projectileTexture->getSize() / _scale);
        std::shared_ptr<Projectile> projectile = Projectile::alloc(Vec2(0, 0), projSize, i);
        _physicsWorld->addObstacle(projectile);
        projectile->setTextures(projectileTexture);
        projectile->setDrawScale(_scale);
        projectile->setActive(false);
        projectile->getSceneNode()->setVisible(false);
        projectile->setDebugColor(Color4::YELLOW);
        projectile->setDebugScene(_debugNode);
        _worldNode->addChild(projectile->getSceneNode());
        _projectiles.push_back(projectile);
    }
    
    Vec2 playerPos = ((Vec2)PLAYER_POS);
    Size playerSize(1, 2);
    for(int i = 0; i < _numPlayers; ++i){
        if (_playerSpawns.size()-1>=i) {
            playerPos = _playerSpawns[i];
        } else {
            playerPos = ((Vec2)PLAYER_POS);
        }
        auto player = Player::alloc(playerPos, playerSize, Element::Water);
        player->setSkinKey("player_skin");
        player->setColorKey("player_color");
        player->setFaceKey("player_face");
        player->setBodyKey("player_body_line");
        player->setHatKey("player_hat");
        player->setStaffKey("player_staff");
        player->setRingKey("player_direction");
        
        _physicsWorld->addObstacle(player);
        player->setTextures(_assets);
        player->setDrawScale(_scale);
        player->setDebugScene(_debugNode);
        player->setID(i);
        player->setDebugColor(Color4::YELLOW);
        player->setHoldingEgg(false);
        player->setOrbScore(0);
        //player id is set to i right now, if that is changed, projectile's associated userid needs to change too
        player->setProjectile(_projectiles[i]);
        player->allocUsernameNode(_assets->get<Font>("username"));
        _worldNode->addChild(player->getSceneNode(),1);
        _players.push_back(player);
    }
    
    for(auto it = _decorations.begin(); it!= _decorations.end();  ++it) {
        std::string name = get<0>(*it);
        Vec2 pos = get<1>(*it);
        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(name));
        sprite->setPosition(pos*_scale);
        _worldNode->addChild(sprite,2);
    }
    
}

// Used to reset the scene
void World::clearRootNode() {
    if (!_root) {
        return;
    }
    _worldNode->removeFromParent();
    _worldNode->removeAllChildren();
    _worldNode = nullptr;
  
    _debugNode->removeFromParent();
    _debugNode->removeAllChildren();
    _debugNode = nullptr;

    _root = nullptr;
    _players.clear();
    _eggs.clear();
    _orbs.clear();
    _orbSpawns.clear();
    _eggSpawns.clear();
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
    
    auto walls = json->get(WALLS_FIELD);
    if (walls != nullptr) {
        loadWalls(walls);
    } else {
        CUAssertLog(false, "Failed to load walls");
        return false;
    }
    
    
    /* auto tiles = json->get(TILES_FIELD);
    if (tiles != nullptr) {
        int tsize = (int)tiles->size();
        for(int ii = 0; ii < tsize; ii++) {
            loadBackground(tiles->get(ii));
        }
    } else {
        CUAssertLog(false, "Failed to load tiles");
        return false;
    } */


    auto decorations = json->get(DECORATIONS_FIELD);
    if (decorations != nullptr) {
        int dsize = (int)decorations->size();
        for(int ii = 0; ii < dsize; ii++) {
            loadDecoration(decorations->get(ii));
        }
    } else {
        CUAssertLog(false, "Failed to load decorations");
        return false;
    }
    
    return true;
}


//Used to unload objects for memory, likely will not need for now.
void World::unload()  {
    CULog("Unloading world");
    for (auto it = _players.begin(); it != _players.end(); ++it)  {
        if (_physicsWorld != nullptr  && (*it).get()->getBody() != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _players.clear();
    
    for (auto it = _swapStations.begin(); it != _swapStations.end(); ++it)  {
        if (_physicsWorld != nullptr && (*it).get()->getBody() != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _swapStations.clear();

    for (auto it = _boosters.begin(); it != _boosters.end(); ++it) {
        if (_physicsWorld != nullptr && (*it).get()->getBody() != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _boosters.clear();
    
    for (auto it = _eggs.begin(); it != _eggs.end(); ++it)  {
        if (_physicsWorld != nullptr && (*it).get()->getBody() != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _eggs.clear();
    
    for (auto it = _orbs.begin(); it != _orbs.end(); ++it)  {
        if (_physicsWorld != nullptr && (*it).get()->getBody() != nullptr) {
            _physicsWorld->removeObstacle((*it).get());
        }
        (*it) = nullptr;
    }
    _orbs.clear();
    
    for (auto it = _walls.begin(); it != _walls.end(); ++it)  {
        if (_physicsWorld != nullptr && (*it).get()->getBody() != nullptr) {
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

#pragma mark -
#pragma mark Object Loading
// Parsing through JSON file and populating world variables

bool World::loadBackground(const std::shared_ptr<JsonValue> &json) {
    float xCoord = json->getFloat(X_FIELD) * globals::TILE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::TILE_TO_BOX2D;
    
    std::string assetName = json->getString("asset");
    
    _bgTiles.push_back(std::make_tuple(assetName,Vec2(xCoord,yCoord)));
    
    return true;
}


bool World::loadWalls(const std::shared_ptr<JsonValue> &json) {
    bool success = true;
    CULog("loading wall");
    
    std::string wname = "wall";
    
    auto wsize = json->size();
    std::vector<Vec2> points;
    for (int ii = 0; ii<wsize ; ii++) {
        for (int jj = 0; jj<json->get(ii)->size(); jj++) {
            points.push_back(Vec2(json->get(ii)->get(jj)->getFloat("x")*globals::SCENE_TO_BOX2D,json->get(ii)->get(jj)->getFloat("y")*globals::SCENE_TO_BOX2D));
        }
        std::shared_ptr<physics2::PolygonObstacle> wallobj;
        Poly2 wall(points);
        SimpleTriangulator triangulator;
        triangulator.set(wall);
        triangulator.calculate();
        wall.setIndices(triangulator.getTriangulation());
        wall.setGeometry(Geometry::SOLID);

        wallobj = physics2::PolygonObstacle::alloc(wall);
        wallobj->setDebugColor(Color4::WHITE);
        // You cannot add constant "".  Must stringify
        wallobj->setName(std::string("wall")+cugl::strtool::to_string(ii));
        wallobj->setName(wname);
        wallobj->setFriction(0.0);
        wallobj->setRestitution(0.6);
        // Set the physics attributes
        wallobj->setBodyType(b2_staticBody);

        wall *= _scale;
        _walls.push_back(wallobj);
        points.clear();
    }
    
    return success;
}

bool World::loadDecoration(const std::shared_ptr<JsonValue> &json) {
    float xCoord = json->getFloat(X_FIELD) * globals::SCENE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::SCENE_TO_BOX2D;
    
    std::string assetName = json->getString("asset");
    
    _decorations.push_back(std::make_tuple(assetName,Vec2(xCoord,yCoord)));
    
    return true;
}

bool World::loadStation(const std::shared_ptr<JsonValue> &json) {
    float xCoord = json->getFloat(X_FIELD) * globals::SCENE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::SCENE_TO_BOX2D;
    
    // **** NEED TO CHANGE SIZE, CANNOT ACCESS _ASSETS IN LOADS
    Vec2 swapStPos = Vec2(xCoord,yCoord);
    auto swapStation = SwapStation::alloc(swapStPos);
    _swapStations.push_back(swapStation);
    return true;
}

bool World::loadBooster(const std::shared_ptr<JsonValue>& json) {
    float xCoord = json->getFloat(X_FIELD) * globals::SCENE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::SCENE_TO_BOX2D;

    // **** NEED TO CHANGE SIZE, CANNOT ACCESS _ASSETS IN LOADS
    Vec2 boosterPos = Vec2(xCoord, yCoord);
    auto boostpad = Booster::alloc(boosterPos);
    _boosters.push_back(boostpad);
    return true;
}

bool World::loadEgg(const std::shared_ptr<JsonValue> &json){
    float xCoord = json->getFloat(X_FIELD) * globals::SCENE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::SCENE_TO_BOX2D;
    
    // **** NEED TO CHANGE SIZE, CANNOT ACCESS _ASSETS IN LOADS
    
//    auto eggTexture = _assets->get<Texture>("target");
    Vec2 eggPos = Vec2(xCoord,yCoord);
//    Size eggSize(eggTexture->getSize() / _scale);
    Size eggSize(1, 2);
    auto egg = Egg::alloc(eggPos, eggSize);
    CULog("orbPos: %f   y %f",eggPos.x,eggPos.y);

    _initEggLocs.push_back(eggPos);
    _eggSpawns.push_back(eggPos);
    return true;
}


bool World::loadOrb(const std::shared_ptr<JsonValue> &json){
    float xCoord = json->getFloat(X_FIELD) * globals::SCENE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::SCENE_TO_BOX2D;
    
    // **** NEED TO CHANGE SIZE, CANNOT ACCESS _ASSETS IN LOADS
    
//    auto eggTexture = _assets->get<Texture>("target");
    Vec2 orbPos = Vec2(xCoord,yCoord);
//    Size orbSize(orbTexture->getSize() / _scale);
    Size orbSize(1, 2);
    auto orb = Orb::alloc(orbPos);
    _orbs.push_back(orb);
    return true;
}

bool World::loadOrbLoc(const std::shared_ptr<JsonValue> &json){
    float xCoord = json->getFloat(X_FIELD) * globals::SCENE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::SCENE_TO_BOX2D;
    
    Vec2 orbPos = Vec2(xCoord,yCoord);
    
    _initOrbLocs.push_back(orbPos);
    return true;
}


//Only get spawn locations from json. wait to load players until numplayers
bool World::loadPlayerSpawn(const std::shared_ptr<JsonValue> &json) {
    float xCoord = json->getFloat(X_FIELD) * globals::SCENE_TO_BOX2D;
    float yCoord = json->getFloat(Y_FIELD) * globals::SCENE_TO_BOX2D;
    
    Vec2 spawnPos = Vec2(xCoord,yCoord);
    
    _playerSpawns.push_back(spawnPos);
    return true;
}

GameObjectType World::getObjectType(std::string obj) {
    if (obj == SWAP_STATION) {
        return GameObjectType::Station;
    } else if (obj == BOOSTER) {
        return GameObjectType::Booster;
    } else if (obj == EGG_SPAWN) {
        return GameObjectType::EggSpawn;
    }  else if (obj == ORB_SPAWN) {
        return GameObjectType::OrbSpawn;
    } else if (obj == ORB_LOCATION) {
        return GameObjectType::OrbLocation;
    } else {
        return GameObjectType::PlayerSpawn;
    }
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

        case GameObjectType::Booster:
            success = loadBooster(json);
            break;
            
        case GameObjectType::EggSpawn:
            success = loadEgg(json);
            break;
            
        case GameObjectType::OrbSpawn:
            success = loadOrb(json);
            break;
            
        case  GameObjectType::OrbLocation:
            success = loadOrbLoc(json);
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

