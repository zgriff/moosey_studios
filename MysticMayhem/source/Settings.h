//
//  Settings.h
//  MysticMayhem
//
//  Created by Ikra Monjur on 5/2/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef Settings_h
#define Settings_h

#include <cugl/cugl.h>

class Settings : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // VIEW
    std::shared_ptr<cugl::scene2::Slider> _musicVolume;
    std::shared_ptr<cugl::scene2::Slider> _soundVolume;
    std::shared_ptr<cugl::scene2::Button> _backButton;
    
    bool _back;
    
    
    
public:
#pragma mark -
#pragma mark Constructors
    
    Settings() : cugl::Scene2() {}
    
    
    ~Settings() { dispose(); }
    
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
//    void update(float timestep);
    
    bool backPressed() { return _back; }

};

#endif /* Settings_h */