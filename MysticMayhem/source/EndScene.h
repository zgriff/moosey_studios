//
//  EndScene.h
//  Roshamboogie
//
//  Created by Ikra Monjur on 4/3/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef EndScene_h
#define EndScene_h

#include <cugl/cugl.h>

class EndScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // VIEW
    /** The host game button */
    std::shared_ptr<cugl::scene2::Button> _playAgainButton;
    
    

public:
#pragma mark -
#pragma mark Constructors
    
    EndScene() : cugl::Scene2() {}
    
    
    ~EndScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    
#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    bool isPending() const;

};


#endif /* EndScene_h */
