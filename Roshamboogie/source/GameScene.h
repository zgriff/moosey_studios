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
#include "TapMoveInputController.h"
#include "Player.h"
#include "Orb.h"
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>

class GameScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
    /** The Box2D world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;
    
    /**
     * Activates the UI elements to make them interactive
     *
     * The elements do not actually do anything.  They just visually respond
     * to input.
     */
    void activateUI(const std::shared_ptr<cugl::scene2::SceneNode>& scene);
    
    // Attach input controllers directly to the scene (no pointers)
    /** Controller for the player */
    //InputController _playerController;
    TapMoveInputController _playerController;

    /** Location and animation information for player (MODEL CLASS) */
    std::shared_ptr<Player> _player;
    
    std::shared_ptr<Orb> _orbTest;
    
    /** The weapon fire sound for the blue player */
//    std::shared_ptr<cugl::Sound> _blueSound;
    bool orbShouldMove = false;
    
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
    
    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;
    
    void moveOrb(Orb* orb);
    
#pragma mark -
#pragma mark Collision Handling
    void beginContact(b2Contact* contact);

    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);

};

#endif /* GameScene_hpp */
