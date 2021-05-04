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
#include <time.h>
#include "Element.h"
#include "Projectile.h"
#include <algorithm>

class Player : public cugl::physics2::CapsuleObstacle{
private:
    CU_DISALLOW_COPY_AND_ASSIGN(Player);
    
    cugl::Vec2 _force;
    int _id;
    Element _currElt;
    Element _prevElt; //used when switching back to the element before collecting an egg
    bool _isTagged; //if this player is tagged
    bool _didTag; //if this player tagged someone else -- earn points
    int _score;
    bool _isInvisible;
    bool _isIntangible; //can't interact with any object (can't tag and nobody can tag you)
    time_t _tagCooldown;
    bool _holdingEgg;
    bool _justHitByProjectile = false;
    bool _isLocal; // true if the player is the one running on this system
    int _eggID; //id of the egg that the player is holding, if any
    cugl::Vec2 _positionError;
    bool _horizFlip;
    
    std::unordered_map<std::string,std::shared_ptr<cugl::scene2::AnimationNode>> _animNodes;
    std::unordered_map<std::string,bool> _animCycles;
    std::unordered_map<std::string,int> _frameNumbers;
    
    
    /** Animation Nodes :o */
    std::string _skinKey;
    std::string _colorKey;
    std::string _faceKey;
    std::string _bodyKey;
    std::string _hatKey;
    std::string _staffKey;
    std::string _ringKey;
    
    clock_t _animationTimer;
    clock_t _animationRate = 0.1 * CLOCKS_PER_SEC;
        
    /** Cache object for transforming the force according the object angle */
    cugl::Mat4 _affine;
    float _drawscale;
    /** severity of screenshake*/
    float _trauma = 0.0;
    /** the direction the player is pointing
        0.0 directed in the positive x axis
        adding rotates counterclockwise      */
    float _direct = 0.0;
    
    // Asset references.  These should be set by GameMode
    /** Reference to the node for the player */
    std::shared_ptr<cugl::scene2::PolygonNode> _sceneNode;
    /** Reference to player's sprite for drawing */
    std::shared_ptr<cugl::scene2::AnimationNode> _animationNode;

    std::string _username;
    std::shared_ptr<cugl::scene2::Label> _usernameNode;

    /** Reference to the player texture */
    std::shared_ptr<cugl::Texture> _texture;

    int _orbScore = 0;
    // Associating each player with their own projectile makes it easier to network probably
    std::shared_ptr<Projectile> _projectile;

public:
#pragma mark Properties
    enum class AssetNodes {
        SKIN, COLOR, FACE, BODY, HAT, STAFF
    };
    
    
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
    
    Element getCurrElement() { return _currElt; }
    
    Element getPrevElement() {
        return _prevElt; }
    
    Element getPreyElement();
    
    bool getIsTagged() { return _isTagged; }
    
    void setIsTagged(bool t) { _isTagged = t; }
    
    bool getDidTag() { return _didTag; }
    
    void setDidTag(bool t) { _didTag = t; }
    
    int getScore() { return _score; }
    
    void incScore(int s) { _score = _score + s; }
    
    bool getIsInvisible() { return _isInvisible; }
    
    void setIsInvisible(bool b) { _isInvisible = b; }
    
    bool getIsIntangible() { return _isIntangible; }
    
    void setIsIntangible(bool b) { _isIntangible = b; }
    
    time_t getTagCooldown() { return _tagCooldown; }
    
    void setTagCooldown(clock_t t) { _tagCooldown = t; }

    float getDirection() { return _direct; }

    void setDirection(float d);
    
    bool getHoldingEgg() { return _holdingEgg; }
    
    void setHoldingEgg(bool b) { _holdingEgg = b; }

    bool getJustHitByProjectile() { return _justHitByProjectile; };
    void setJustHitByProjectile(bool hit) { _justHitByProjectile = hit; };
    
    bool getIsLocal() { return _isLocal; }
    
    void setIsLocal(bool b) { _isLocal = b; }
    
    cugl::Vec2 getPositionError() { return _positionError; }
    
    void setPositionError(cugl::Vec2 e) { _positionError = e; }
    
    int getEggId() { return _eggID; }
    
    void setEggId(int eid) {
        _eggID = eid;
    }

    void setOrbScore(int orbScore) { _orbScore = min(orbScore, 5); };

    int getOrbScore() { return _orbScore; };

    std::shared_ptr<Projectile> getProjectile() { return _projectile; };

    void setProjectile(std::shared_ptr<Projectile> projectile) { _projectile = projectile; };

    /**
    * Creates the username Label node with the font
    *
    */
    void allocUsernameNode(const std::shared_ptr<cugl::Font>& font);

    void setUsername(std::string name) { _username = name; };
    
    int calculateFrame(int frame, std::string key);

    
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
    void setTextures(const std::shared_ptr<cugl::AssetManager>& assets);
    
    
    const std::shared_ptr<cugl::Texture> getTexture() const {
        return _texture;
    }
    
    void setSkinKey(std::string skin) {
        _skinKey = skin;
    }
    
    void setColorKey(std::string color) {
        _colorKey = color;
    }
    
    void setFaceKey(std::string face) {
        _faceKey = face;
    }
    
    void setBodyKey(std::string body) {
        _bodyKey = body;
    }
    
    void setHatKey(std::string hat) {
        _hatKey = hat;
    }
    
    void setStaffKey(std::string staff) {
        _staffKey = staff;
    }
    
    void setRingKey(std::string ring) {
        _ringKey = ring;
    }
    
    
#pragma mark Constructors
    /**
     * Creates a player with the default values.
     *
     * To properly initialize the player, you should call the init
     * method.
     */
    Player(void) : CapsuleObstacle(), _drawscale(1.0f), _isLocal(false) { }
    
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
    /**
    * Returns the Player's current trauma value
    */
    float getTrauma() { return _trauma; }

    /**
    * Increases the Player's current trauma by t
    *
    * Trauma cannot exceed a threshold of 1.0f
    */
    void addTrauma(float t);
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
    
#pragma mark -
#pragma mark Animation
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
    
    
    
    void animateMovement();
    
    void animationCycle(cugl::scene2::AnimationNode* node, bool* cycle, std::string key);
    
    void flipHorizontal(bool flip);
    
    
};

#endif
