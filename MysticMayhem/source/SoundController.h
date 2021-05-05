//
//  SoundController.h
//  MysticMayhem
//
//  Created by Joshua Kaplan on 4/23/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef SoundController_h
#define SoundController_h

#include <cugl/cugl.h>

namespace SoundController {

enum class Type {
    ORB, EGG, TAG, SWAP
};

//call this before calling any other SoundController method
void init(std::shared_ptr<cugl::AssetManager> assets);

//play a sound at given position
//pos is relative to the player, with (0,0) being on the player
void playSound(Type s, cugl::Vec2 pos);

//void playMusic();

//void pauseMusic();
void setSoundVolume(float volume);

//Call this method to enable or disable spatial audio
// true will enable it, false will disable it
void useSpatialAudio(bool useSpatialAudio);

}
#endif /* SoundController_h */
