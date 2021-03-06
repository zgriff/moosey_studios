//
//  Booster.hpp
//  Roshamboogie
//
//  Created by Thor Andreassen on 4/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __BOOSTER_H__
#define __BOOSTER_H__

#include <cugl/cugl.h>
#include <time.h>


using namespace cugl;

class Booster : public cugl::physics2::BoxObstacle{
private:
    
    /** Cache object for transforming the force according the object angle */
    cugl::Mat4 _affine;
    float _drawscale;

    
    // Asset references.  These should be set by GameMode
    /** Reference to the node for the booster */
    std::shared_ptr<cugl::scene2::SceneNode> _sceneNode;
    std::shared_ptr<cugl::scene2::AnimationNode> _animationNode;
    
    /** Reference to the booster texture */
    std::shared_ptr<cugl::Texture> _texture;
    bool _active;
    time_t _lastUsed;
    time_t _coolDownSecs = 5;
    int _id;
    
public:
    
    /**
     * Sets the textures for this booster station.
     *
     * @param boostText      The texture for the ship filmstrip
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& boostText);
    
    
    const std::shared_ptr<cugl::Texture> getTexture() const {
        return _texture;
    }
    
    std::shared_ptr<cugl::scene2::SceneNode> getSceneNode(){
        return _sceneNode;
    }
    
    bool getActive() {return _active; }
    
    void setActive(bool a) override { _active = a; }
    
    time_t getLastUsed() { return _lastUsed; }
    
    void setLastUsed(time_t time) { _lastUsed = time; }
    
    time_t getCoolDown() { return _coolDownSecs; }
    
    int getID(){ return _id; }
    void setID(int i){ _id = i; }
    

    
    
#pragma mark Constructors
    /**
     * Creates a booster with the default values.
     *
     * To properly initialize the booster, you should call the init
     * method.
     */
    Booster(void) : BoxObstacle(), _drawscale(1.0f) { }
    
    /**
     * Disposes the booster, releasing all resources.
     */
    virtual ~Booster(void) { dispose(); }

    /**
     * Disposes the booster, releasing all resources.
     */
    void dispose();
    
    /**
     * Initializes a new booster
     *     *
     * @return true if the initialization was successful
     */
    virtual bool init(const cugl::Vec2 pos) override;

    /**
     * Returns a newly allocated  swap station
     *
     *
     * @return a newly allocated  swap station
     */
    static std::shared_ptr<Booster> alloc(const cugl::Vec2 pos) {
        std::shared_ptr<Booster> result = std::make_shared<Booster>();
        return (result->init(pos) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Physics
    
    /**
     * Sets the ratio of the booster sprite to the physics body
     *
     *
     * @param scale The ratio of the booster sprite to the physics body
     */
    void setDrawScale(float scale);
    
    /**
     * Returns the ratio of the booster sprite to the physics body
     *
     *
     * @return the ratio of the booster sprite to the physics body
     */
    float getDrawScale() const { return _drawscale; }
    

    /*
     @param delta Timing values from parent loop
    */
    virtual void update(float delta) override;
    
};


#endif /* Booster_hpp */
