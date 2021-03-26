//
//  GameScene.hpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include <time.h>
#include "InputController.h"
#include "CollisionController.h"
#include "Player.h"
#include "Orb.h"
#include "NetworkController.h"
#include "SwapStation.h"
#include "Egg.h"
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "World.h"

class GameScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

    /** Reference to the physics root of the scene graph */
//    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
    /** The Box2D world */
//    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    
    std::shared_ptr<World> world;
    
    std::shared_ptr<cugl::scene2::Label> _hatchnode;
    
    std::shared_ptr<cugl::scene2::ProgressBar>  _hatchbar;
    
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _debugnode;
    
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

    std::shared_ptr<cugl::scene2::Label> _roomIdHUD;
    std::string _currRoomId;
    
    clock_t _hatchTextTimer = CLOCKS_PER_SEC;
    clock_t _hatchedTime;
    
    /** Whether or not debug mode is active */
    bool _debug;
    /**
     * Activates the UI elements to make them interactive
     *
     * The elements do not actually do anything.  They just visually respond
     * to input.
     */
    void activateUI(const std::shared_ptr<cugl::scene2::SceneNode>& scene);
    
    // Attach input controllers directly to the scene (no pointers)
    /** Controller for the player */
    InputController _playerController;

    /** Location and animation information for player (MODEL CLASS) */
//    std::shared_ptr<Player> _player;
//    std::shared_ptr<Player> _player2;
//
//    std::shared_ptr<Orb> _fireOrb;
//    std::shared_ptr<Orb> _waterOrb;
//    std::shared_ptr<Orb> _grassOrb;
//
//    std::shared_ptr<SwapStation> _swapStation;
//
//    std::shared_ptr<Egg> _egg;
    
    std::shared_ptr<cugl::scene2::Label> _scoreHUD;
    
    int _score = 0;
    
    
    /** The weapon fire sound for the blue player */
//    std::shared_ptr<cugl::Sound> _blueSound;
    bool swap = false;
    
    std::string updateScoreText(const int score);
    
    bool isDebug( ) const { return _debug; }
    
    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; _debugnode->setVisible(value); }

    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;


    /**
     * Resets the status of the game so that we can play again.
     */
    void reset() override;
    
#pragma mark Internal Object Management
    /**
     * Lays out the game geography.
     *
     * Pay close attention to how we attach physics objects to a scene graph.
     * The simplest way is to make a subclass, like we do for the dude.  However,
     * for simple objects you can just use a callback function to lightly couple
     * them.  This is what we do with the crates.
     *
     * This method is really, really long.  In practice, you would replace this
     * with your serialization loader, which would process a level file.
     */
    void populate();

    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object, like we did
     * with dude.  The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     *
     * In addition, scene graph nodes have a z-order.  This is the order they are
     * drawn in the scene graph node.  Objects with the different textures should
     * have different z-orders whenever possible.  This will cut down on the
     * amount of drawing done
     *
     * @param obj    The physics object to add
     * @param node   The scene graph node to attach it to
     * @param zOrder The drawing order
     * @param useObjPosition  Whether to update the node's position to be at the object's position
     */
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                     const std::shared_ptr<cugl::scene2::SceneNode>& node,
                     int zOrder, bool useObjPosition=true);
    
    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;
    
    void setMovementStyle(int m);
    
    void moveOrb(Orb* orb);

};

#endif /* GameScene_hpp */
