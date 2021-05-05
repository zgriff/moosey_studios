//
//  SwapStation.hpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __SWAP_STATION_H__
#define __SWAP_STATION_H__

#include <cugl/cugl.h>
#include <time.h>


using namespace cugl;

class SwapStation : public cugl::physics2::WheelObstacle{
private:
    
    /** Cache object for transforming the force according the object angle */
    cugl::Mat4 _affine;
    float _drawscale;

    
    // Asset references.  These should be set by GameMode
    /** Reference to the node for the swap station */
    std::shared_ptr<cugl::scene2::SceneNode> _sceneNode;
    std::shared_ptr<cugl::scene2::AnimationNode> _animationNode;
    
    /** Reference to the swap station texture */
    std::shared_ptr<cugl::Texture> _texture;
    bool _active;
    int _lastUser;
    time_t _lastUsed;
    time_t _coolDownSecs = 5;
    time_t _coolDownSecsDeath = 0.25;
    int _id;
    
public:
    
    /**
     * Sets the textures for this swap station.
     *
     * @param swapText      The texture for the ship filmstrip
     */
    void setTextures(const std::shared_ptr<cugl::Texture>& swapText);
    
    
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
    
    void setLastUser(int pid) {_lastUser = pid;}
    
    int getLastUser() {return _lastUser;}
    
    int getID(){ return _id; }
    void setID(int i){ _id = i; }
    

    
    
#pragma mark Constructors
    /**
     * Creates a swap station with the default values.
     *
     * To properly initialize the swap station, you should call the init
     * method.
     */
    SwapStation(void) : WheelObstacle(), _drawscale(1.0f) { }
    
    /**
     * Disposes the swap station, releasing all resources.
     */
    virtual ~SwapStation(void) { dispose(); }

    /**
     * Disposes the  swap station, releasing all resources.
     */
    void dispose();
    
    /**
     * Initializes a new  swap station
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
    static std::shared_ptr<SwapStation> alloc(const cugl::Vec2 pos) {
        std::shared_ptr<SwapStation> result = std::make_shared<SwapStation>();
        return (result->init(pos) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Physics
    
    /**
     * Sets the ratio of the swap station sprite to the physics body
     *
     *
     * @param scale The ratio of the swap station sprite to the physics body
     */
    void setDrawScale(float scale);
    
    /**
     * Returns the ratio of the swap station sprite to the physics body
     *
     *
     * @return the ratio of the swap station sprite to the physics body
     */
    float getDrawScale() const { return _drawscale; }
    

    /*
     @param delta Timing values from parent loop
    */
    virtual void update(float delta) override;
    
};


#endif /* SwapStation_hpp */
