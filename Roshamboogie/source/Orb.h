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
    Element e;
    float _drawscale;
    std::shared_ptr<cugl::scene2::SceneNode> _sceneNode;
    std::shared_ptr<cugl::Texture> _texture;
public:
    Orb(void) : WheelObstacle(), _drawscale(1.0f) { }
    
    virtual ~Orb(void){
        dispose();
    }
    void dispose();
    
    virtual bool init(Element e);
    
    static std::shared_ptr<Orb> alloc(const Element e) {
        std::shared_ptr<Orb> result = std::make_shared<Orb>();
        return (result->init(e) ? result : nullptr);
    }
    
    void setTextures(const std::shared_ptr<cugl::Texture>& orb);
    
    Element getElement(){
        return e;
    }
    
    
    
    std::shared_ptr<cugl::scene2::SceneNode> getSceneNode(){
        return _sceneNode;
    }
    void setDrawScale(float scale);
    
    virtual void update(float delta) override;
};

#endif /* Orb_hpp */