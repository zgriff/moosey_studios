//
//  Egg.h
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/17/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef Egg_h
#define Egg_h

#include <cugl/cugl.h>

using namespace cugl;

class Egg : public cugl::physics2::BoxObstacle{
private:
    
    /** Cache object for transforming the force according the object angle */
    cugl::Mat4 _affine;
    float _drawscale;

    // Asset references.  These should be set by GameMode
    /** Reference to the node for the egg */
    std::shared_ptr<cugl::scene2::SceneNode> _sceneNode;
    
    /** Reference to the egg texture */
    std::shared_ptr<cugl::Texture> _texture;
    
    bool _hatched;
    bool _collected;
    Vec2 _initPos;
    Vec2 _spawnLoc; //where the egg spawned
    float _distanceWalked;
    int _id;
    int _playerID; //id of the player who collected the egg
    
public:
    
    /**
     * Sets the textures for this egg.
     *
     * @param eggText      The texture for the egg
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& eggText);
    
    
    const std::shared_ptr<cugl::Texture> getTexture() const {
        return _texture;
    }
    
    std::shared_ptr<cugl::scene2::SceneNode> getSceneNode(){
        return _sceneNode;
    }
    
    bool getHatched() { return _hatched; }
    
    void setHatched(bool h) { _hatched = h; }
    
    bool getCollected() { return _collected; }
    
    void setCollected(bool b) { _collected = b; }
    
    int getID(){ return _id; }
    void setID(int i){ _id = i; }
    
    int getPID(){ return _playerID; }
    
    void setPID(int i){ _playerID = i; }
    
    Vec2 getInitPos() { return _initPos; }
    
    void setInitPos(Vec2 pos) { _initPos = pos; }
    
    Vec2 getSpawnLoc() { return _spawnLoc; }
    
    void setSpawnLoc(Vec2 loc) {_spawnLoc = loc; }
    
    float getDistanceWalked() { return _distanceWalked; }
    
    void setDistanceWalked(float dist) { _distanceWalked = dist; }
 
    void update(float delta) override;
    
    
#pragma mark Constructors
    /**
     * Creates an egg with the default values.
     *
     * To properly initialize the swap station, you should call the init
     * method.
     */
    Egg(void) : BoxObstacle(), _drawscale(1.0f) { }
    
    /**
     * Disposes the egg, releasing all resources.
     */
    virtual ~Egg(void) { dispose(); }

    /**
     * Disposes the  egg, releasing all resources.
     */
    void dispose();
    
    /**
     * Initializes a new  egg
     *     *
     * @return true if the initialization was successful
     */
    virtual bool init(const cugl::Vec2 pos, const cugl::Size size) override;

    /**
     * Returns a newly allocated  egg
     *
     *
     * @return a newly allocated  egg
     */
    static std::shared_ptr<Egg> alloc(const cugl::Vec2 pos, const cugl::Size size) {
        std::shared_ptr<Egg> result = std::make_shared<Egg>();
        return (result->init(pos, size) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Physics
    
    /**
     * Sets the ratio of the sprite to the physics body
     *
     *
     * @param scale The ratio of the  sprite to the physics body
     */
    void setDrawScale(float scale);
    
    /**
     * Returns the ratio of the sprite to the physics body
     *
     *
     * @return the ratio of the sprite to the physics body
     */
    float getDrawScale() const { return _drawscale; }
    
};

#endif /* Egg_h */
