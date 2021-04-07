//
//  World.h
//  Roshamboogie
//
//  Created by Joshua Kaplan on 3/23/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef World_h
#define World_h

#include <cugl/cugl.h>
#include <cugl/assets/CUAsset.h>
#include <cugl/io/CUJsonReader.h>
#include "Player.h"
#include "Orb.h"
#include "SwapStation.h"
#include "Egg.h"
#include "MapConstants.h"



class World : public Asset {
protected:
    /** The root node of this level */
    std::shared_ptr<scene2::SceneNode> _root;
    
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The bounds of this level in physics coordinates */
    Rect _bounds;
    /** The level drawing scale (difference between physics and drawing coordinates) */
    Vec2 _scale;
    
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _worldNode;
    
    std::shared_ptr<cugl::scene2::SceneNode> _debugNode;
    
    /** The Box2D world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _physicsWorld;
    
    std::vector<std::shared_ptr<physics2::PolygonObstacle>> _walls;
    
    std::vector<Vec2> _playerSpawns;
    
    std::vector<std::shared_ptr<Player>> _players;
    
    std::vector<std::shared_ptr<Orb>> _orbs;
    
    std::vector<std::shared_ptr<SwapStation>> _swapStations;
    
    std::vector<std::shared_ptr<Egg>> _eggs;
    
    bool _debug;
    
//    float _scale;
    uint8_t _numPlayers;
    
#pragma mark Internal Helpers
    bool loadPlayer(const int i, Vec2 loc);
    
    bool loadPlayerSpawn(const std::shared_ptr<JsonValue>& json);
    
    bool loadWall(const std::shared_ptr<JsonValue>& json);
    
    bool loadGameObject(const std::shared_ptr<JsonValue>& json);
    
    bool loadOrb(const std::shared_ptr<JsonValue>& json);
    
    bool loadStation(const std::shared_ptr<JsonValue>& json);
    
    bool loadEgg(const std::shared_ptr<JsonValue>& json);
    
    bool loadBackground(const std::shared_ptr<JsonValue>& json);
    
    bool loadDecorations(const std::shared_ptr<JsonValue>& json);
    
    Color4 parseColor(std::string name);
    
    GameObjectType getObjectType(std::string obj);
    
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                     const std::shared_ptr<cugl::scene2::SceneNode>& node, int zOrder);
    
    /**
     * Clears the root scene graph node for this level
     */
    void clearRootNode();

    

public:
#pragma mark -
#pragma mark Constructors
//    World(){}
//    ~World() { dispose(); }
//    void dispose();
    
//    bool init(int width, int height);
//
//    static std::shared_ptr<World> alloc(const std::shared_ptr<cugl::AssetManager>& assets, int width, int height, float scale, uint8_t numPlayers){
//        std::shared_ptr<World> result = std::make_shared<World>();
//        result->_assets = assets;
//        result->_scale = scale;
//        result->_numPlayers = numPlayers;
//        return (result->init(width, height) ? result : nullptr);
//    }
//
    /**
     * Creates a new game level with no source file.
     *
     * The source file can be set at any time via the setFile() method. This method
     * does NOT load the asset.  You must call the load() method to do that.
     *
     * @return  an autoreleased level file
     */
    static std::shared_ptr<World> alloc() {
        std::shared_ptr<World> result = std::make_shared<World>();
        return (result->init("") ? result : nullptr);
    }

    /**
     * Creates a new game level with the given source file.
     *
     * This method does NOT load the level. You must call the load() method to do that.
     * This method returns false if file does not exist.
     *
     * @return  an autoreleased level file
     */
    static std::shared_ptr<World> alloc(std::string file) {
        std::shared_ptr<World> result = std::make_shared<World>();
        return (result->init(file) ? result : nullptr);
    }
    
    
#pragma mark -
#pragma mark Gameplay Handling
    void reset();
    
#pragma mark Drawing Methods
    /**
     * Returns the drawing scale for this game level
     *
     * The drawing scale is the number of pixels to draw before Box2D unit. Because
     * mass is a function of area in Box2D, we typically want the physics objects
     * to be small.  So we decouple that scale from the physics object.  However,
     * we must track the scale difference to communicate with the scene graph.
     *
     * We allow for the scaling factor to be non-uniform.
     *
     * @return the drawing scale for this game level
     */
    const Vec2& getDrawScale() const { return _scale; }

    /**
     * Sets the drawing scale for this game level
     *
     * The drawing scale is the number of pixels to draw before Box2D unit. Because
     * mass is a function of area in Box2D, we typically want the physics objects
     * to be small.  So we decouple that scale from the physics object.  However,
     * we must track the scale difference to communicate with the scene graph.
     *
     * We allow for the scaling factor to be non-uniform.
     *
     * @param value  the drawing scale for this game level
     */
    void setDrawScale(float value);

    /**
     * Returns the scene graph node for drawing purposes.
     *
     * The scene graph is completely decoupled from the physics system.  The node
     * does not have to be the same size as the physics body. We only guarantee
     * that the node is positioned correctly according to the drawing scale.
     *
     * @return the scene graph node for drawing purposes.
     */
    const std::shared_ptr<scene2::SceneNode>& getRootNode() const { return _root; }

    /**
     * Sets the scene graph node for drawing purposes.
     *
     * The scene graph is completely decoupled from the physics system.  The node
     * does not have to be the same size as the physics body. We only guarantee
     * that the node is positioned correctly according to the drawing scale.
     *
     * @param value  the scene graph node for drawing purposes.
     *
     * @retain  a reference to this scene graph node
     * @release the previous scene graph node used by this object
     */
    void setRootNode(const std::shared_ptr<scene2::SceneNode>& root);

    /**
     * Sets the loaded assets for this game level
     *
     * @param assets the loaded assets for this game level
     */
    void setAssets(const std::shared_ptr<AssetManager>& assets) { _assets = assets;  }

    /**
     * Toggles whether to show the debug layer of this game world.
     *
     * The debug layer displays wireframe outlines of the physics fixtures.
     *
     * @param  flag whether to show the debug layer of this game world
     */
    void showDebug(bool flag);
    
#pragma mark -
#pragma mark Getters and setters
    std::shared_ptr<Player> getPlayer(int id){
        return _players[id];
    }
    
    std::shared_ptr<Orb> getOrb(int id){
        return _orbs[id];
    }
    
    std::shared_ptr<Egg> getEgg(int id){
        return _eggs[id];
    }
    
    std::shared_ptr<SwapStation> getSwapStation(int id){
        return _swapStations[id];
    }
    
    std::shared_ptr<cugl::scene2::SceneNode> getSceneNode(){
        return _worldNode;
    }
    
    std::shared_ptr<cugl::physics2::ObstacleWorld> getPhysicsWorld(){
        return _physicsWorld;
    }

    const Rect& getBounds() const   { return _bounds; }
    
    void setNumPlayers(uint8_t num) {_numPlayers = num;}
    
    void setDebugNode(const std::shared_ptr<cugl::scene2::SceneNode> & debugNode){
        _debugNode = debugNode;
    }
    
    const std::shared_ptr<cugl::scene2::SceneNode>& getDebugNode() {
        return  _debugNode;
    }
    void setDebug(bool value) { _debug = value; _debugNode->setVisible(value); }
    
    
#pragma mark -
#pragma mark Asset Loading
    /**
     * Loads this game level from the source file
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded in
     * parallel, not in sequence.  If an asset (like a game level) has references to
     * other assets, then these should be connected later, during scene initialization.
     *
     * @param file the name of the source file to load from
     *
     * @return true if successfully loaded the asset from a file
     */
    virtual bool preload(const std::string& file) override;


    /**
     * Loads this game level from a JsonValue containing all data from a source Json file.
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded in
     * parallel, not in sequence.  If an asset (like a game level) has references to
     * other assets, then these should be connected later, during scene initialization.
     *
     * @param json the json loaded from the source file to use when loading this game level
     *
     * @return true if successfully loaded the asset from the input JsonValue
     */
    virtual bool preload(const std::shared_ptr<cugl::JsonValue>& json) override;

    /**
     * Unloads this game level, releasing all sources
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded and
     * unloaded in parallel, not in sequence.  If an asset (like a game level) has
     * references to other assets, then these should be disconnected earlier.
     */
    void unload();


    //#pragma mark -
    //#pragma mark Initializers
    /**
     * Creates a new, empty level.
     */
    World(void);

    /**
     * Destroys this level, releasing all resources.
     */
    virtual ~World(void);
};


#endif /* World_h */
