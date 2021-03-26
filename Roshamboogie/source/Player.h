//
//  player.h
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <cugl/cugl.h>
#include "Element.h"

class Player : public cugl::physics2::BoxObstacle{
private:
    CU_DISALLOW_COPY_AND_ASSIGN(Player);
    
    cugl::Vec2 _force;
    int _id;
    Element currElt;
    Element prevElt; //used when switching back to the element before collecting an egg
    
    /** Cache object for transforming the force according the object angle */
    cugl::Mat4 _affine;
    float _drawscale;
    
    // Asset references.  These should be set by GameMode
    /** Reference to the node for the player */
    std::shared_ptr<cugl::scene2::SceneNode> _sceneNode;
    /** Reference to player's sprite for drawing */
    std::shared_ptr<cugl::scene2::AnimationNode> _animationNode;

    std::string _username;
    std::shared_ptr<cugl::scene2::Label> _usernameNode;

    /** Reference to the player texture */
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

    const cugl::Vec2& getForce() const { return _force; }
    
    void setForce(const cugl::Vec2& value) { _force.set(value); }
    
    void setElement(Element e);
    
    Element getCurrElement() { return currElt; }
    
    Element getPrevElement() { return prevElt; }
    
    Element getPreyElement();

    /**
    * Creates the username Label node with the font
    *
    */
    void allocUsernameNode(const std::shared_ptr<cugl::Font>& font);

    
#pragma mark Graphics
    /**
     * Returns the scene graph node for this player
     *
     * @return the image texture for this player
     */
    const std::shared_ptr<cugl::scene2::SceneNode> getSceneNode() const {
        return _sceneNode;
    }
    
    /**
     * Sets the textures for this player.
     *
     * @param player      The texture for the player filmstrip
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& player);
    
    
    const std::shared_ptr<cugl::Texture> getTexture() const {
        return _texture;
    }
    
#pragma mark Constructors
    /**
     * Creates a player with the default values.
     *
     * To properly initialize the player, you should call the init
     * method.
     */
    Player(void) : BoxObstacle(), _drawscale(1.0f) { }
    
    /**
     * Disposes the player, releasing all resources.
     */
    virtual ~Player(void) { dispose(); }

    /**
     * Disposes the player, releasing all resources.
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
    virtual bool init(const cugl::Vec2 pos, const cugl::Size size, Element elt);

    /**
     * Returns a newly allocated player.
     *
     * This method does NOT create a scene graph node for this player.  You
     * must call setTextures for that.
     *
     * @return a newly allocated player
     */
    static std::shared_ptr<Player> alloc(const cugl::Vec2 pos, const cugl::Size size, Element elt) {
        std::shared_ptr<Player> result = std::make_shared<Player>();
        return (result->init(pos, size, elt) ? result : nullptr);
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
     * Sets the ratio of the player sprite to the physics body
     *
     * The rocket needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @param scale The ratio of the player sprite to the physics body
     */
    void setDrawScale(float scale);
    
    /**
     * Returns the ratio of the player sprite to the physics body
     *
     * The rocket needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @return the ratio of the player sprite to the physics body
     */
    float getDrawScale() const { return _drawscale; }
    
};

#endif
