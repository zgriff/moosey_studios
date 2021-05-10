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

class Settings : public cugl::scene2::SceneNode {
protected:
    // VIEW
    std::shared_ptr<cugl::scene2::Slider> _musicVolume;
    std::shared_ptr<cugl::scene2::Slider> _soundVolume;
    std::shared_ptr<cugl::scene2::Button> _backButton;
    std::shared_ptr<cugl::scene2::Button> _leavegameButton;
    
    bool _back;
    bool _leaveGame;
    bool _inGame;
    
    
    
public:
    /** The asset manager for loading. */
//    std::shared_ptr<cugl::AssetManager> _assets;
#pragma mark -
#pragma mark Constructors
    
    explicit Settings(const std::shared_ptr<cugl::AssetManager> &assets, bool inGame);
    
    
    ~Settings() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, bool inGame);

    
#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
//    void update(float timestep);
    
    bool backPressed() { return _back; }
    
    bool leaveGamePressed() { return _leaveGame; }
    
    void setActive(bool b);
    

};

#endif /* Settings_h */
