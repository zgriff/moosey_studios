//
//  Ship.h
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <cugl/cugl.h>
#include "Element.h"

#pragma mark -
#pragma mark Physics Constants

/**amount to slow player down after swing*/
#define PLAYER_DAMPING      5.0f
#define DEFAULT_THRUST      30.0f



#pragma mark -
#pragma mark Player Model

class Player : public cugl::physics2::BoxObstacle{
private:
    CU_DISALLOW_COPY_AND_ASSIGN(Player);
    
    cugl::Vec2 _force;
    
    /** The ship identifier */
    int _id;
    Element element;
    
    /** Cache object for transforming the force according the object angle */
    cugl::Mat4 _affine;
    float _drawscale;
    
    /** The current directional movement of the character */
    cugl::Vec2  _movement;

    
    // Asset references.  These should be set by GameMode
    /** Reference to the node for the group of the ship and the aiming reticule */
    std::shared_ptr<cugl::scene2::SceneNode> _sceneNode;
    /** Reference to ship's sprite for drawing */
//    std::shared_ptr<cugl::scene2::AnimationNode> _shipNode;
    /** Reference to the ships texture */
    std::shared_ptr<cugl::Texture> _texture;

public:
#pragma mark Properties
    /**
     * Returns the id of the player.
     *
     * @return the id of the player.
     */
    int getID() const {
        return _id;
    }

    /**
     * Sets the id of the player.
     *
     * @param id    The id of the player.
     */
    void setID(int id) {
        _id = id;
    }
    
    float getThrust() const { return DEFAULT_THRUST; }
    
    const cugl::Vec2& getMovement() const { return _movement; }
    
    void setMovement(cugl::Vec2 value) {_movement.set(value);}

    const cugl::Vec2& getForce() const { return _force; }
    
    const float getDamping() const { return PLAYER_DAMPING; }
    
    void setForce(const cugl::Vec2& value) { _force.set(value); }
    
    void setElement(Element e);
    
    Element getElement() { return element; }
    

    
#pragma mark Graphics
    /**
     * Returns the scene graph node for this ship
     *
     * The scene graph node contains the ship, its shadow, and the
     * target. It is centered on the ship itself. This method
     * returns the null pointer if the textures have not yet
     * been set.
     *
     * @return the image texture for this ship
     */
    const std::shared_ptr<cugl::scene2::SceneNode> getSceneNode() const {
        return _sceneNode;
    }
    
    /**
     * Sets the textures for this ship.
     *
     * The two textures are the ship texture and the target texture. The
     * scene graph node associated with this ship is nullptr until these
     * values are set.
     *
     * @param ship      The texture for the ship filmstrip
     * @param target    The texture for the ship target
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& ship);
    
    
    const std::shared_ptr<cugl::Texture> getTexture() const {
        return _texture;
    }
    
#pragma mark Constructors
    /**
     * Creates a ship with the default values.
     *
     * To properly initialize the ship, you should call the init
     * method.
     */
    Player(void) : BoxObstacle(), _drawscale(1.0f) { }
    
    /**
     * Disposes the ship, releasing all resources.
     */
    virtual ~Player(void) { dispose(); }

    /**
     * Disposes the ship, releasing all resources.
     */
    void dispose();
    
    /**
     * Initializes a new player.
     *
     * This method does NOT create a scene graph node for this player.  You
     * must call setTextures for that.
     *
     * @return true if the initialization was successful
     */
    virtual bool init(const cugl::Vec2 pos, const cugl::Size size) override;

    /**
     * Returns a newly allocated player.
     *
     * This method does NOT create a scene graph node for this player.  You
     * must call setTextures for that.
     *
     * @return a newly allocated player
     */
    static std::shared_ptr<Player> alloc(const cugl::Vec2 pos, const cugl::Size size) {
        std::shared_ptr<Player> result = std::make_shared<Player>();
        return (result->init(pos, size) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Physics
    /**
     * Applies the force to the body of this player
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();
    

    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * This method is called AFTER the collision resolution state. Therefore, it
     * should not be used to process actions or any other gameplay information.
     * Its primary purpose is to adjust changes to the fixture, which have to
     * take place after collision.
     *
     * In other words, this is the method that updates the scene graph.  If you
     * forget to call it, it will not draw your changes.
     *
     * @param delta Timing values from parent loop
     */
    virtual void update(float delta) override;
    
    /**
     * Sets the ratio of the ship sprite to the physics body
     *
     * The rocket needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @param scale The ratio of the ship sprite to the physics body
     */
    void setDrawScale(float scale);
    
    /**
     * Returns the ratio of the ship sprite to the physics body
     *
     * The rocket needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @return the ratio of the ship sprite to the physics body
     */
    float getDrawScale() const { return _drawscale; }
    
};

#endif
