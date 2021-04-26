//
//  Orb.hpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __ORB_H__
#define __ORB_H__

#include <cugl/cugl.h>
#include "Element.h"


class Orb : public cugl::physics2::WheelObstacle{
private:
    float _drawscale;
    std::shared_ptr<cugl::scene2::SceneNode> _sceneNode;
    std::shared_ptr<cugl::scene2::AnimationNode> _animationNode;
    std::shared_ptr<cugl::Texture> _texture;
    bool _collected;
    int _id;

public:
    Orb(void) : WheelObstacle(), _drawscale(1.0f) { }
    
    virtual ~Orb(void){
        dispose();
    }
    void dispose();
    
    virtual bool init(cugl::Vec2 pos) override;
    
    static std::shared_ptr<Orb> alloc(cugl::Vec2 pos) {
        std::shared_ptr<Orb> result = std::make_shared<Orb>();
        return (result->init(pos) ? result : nullptr);
    }
    
    void setTextures(const std::shared_ptr<cugl::Texture>& orb);

    
    bool getCollected() { return _collected; }
    
    void setCollected(bool c) { _collected = c; }
    
    
    std::shared_ptr<cugl::scene2::SceneNode> getSceneNode(){
        return _sceneNode;
    }
    void setDrawScale(float scale);
    
    virtual void update(float delta) override;
    
    void respawn();
    
    int getID(){
        return _id;
    }
    void setID(int i){
        _id = i;
    }
};

#endif /* Orb_hpp */
