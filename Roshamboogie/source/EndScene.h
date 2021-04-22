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
    std::shared_ptr<cugl::scene2::Button> _playAgainButton; //returns to same lobby
    std::shared_ptr<cugl::scene2::Button> _mainMenuButton; //returns to main menu
    std::shared_ptr<cugl::scene2::Label> _results;
    std::shared_ptr<cugl::scene2::Label> _message;
    
    std::string _resStr;
    std::tuple<std::string, std::string> _winnerStr;
    bool _playAgain; //whether player clicked play again or not
    bool _mainMenu; //whether player clicked return to main menu or not
    
public:
#pragma mark -
#pragma mark Constructors
    
    EndScene() : cugl::Scene2() {}
    
    
    ~EndScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::string results, std::tuple<std::string, std::string> winner);

    
#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    bool isPending() const;
    
    bool playAgain() {
        return _playAgain;
    }
    
    bool mainMenu() {
        return _mainMenu;
    }
    
//    void displayResults();

};


#endif /* EndScene_h */
