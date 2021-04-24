//
//  SoundController.cpp
//  MysticMayhem
//
//  Created by Joshua Kaplan on 4/23/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "SoundController.h"
#include <cugl/cugl.h>
#include <cstdlib>

#define NUM_ORB_SOUNDS 4
#define NUM_TAG_SOUNDS 3

namespace SoundController{
std::shared_ptr<cugl::AssetManager> _assets;
bool spatialAudioEnabled = false;


void useSpatialAudio(bool useSpatialAudio){
    spatialAudioEnabled = useSpatialAudio;
}

void init(std::shared_ptr<cugl::AssetManager> assets){
    _assets = assets;
}

void playSound(Type s, cugl::Vec2 pos){
    std::shared_ptr<cugl::Sound> sample;
    switch(s){
        case Type::EGG:
            sample = _assets->get<cugl::Sound>("egg");
            break;
        case Type::ORB:
        {
            int soundnum = (rand() % NUM_ORB_SOUNDS) + 1;
            sample = _assets->get<cugl::Sound>("orb"+std::to_string(soundnum));
            break;
        }
        case Type::TAG:
        {
            int soundnum = (rand() % NUM_TAG_SOUNDS) + 1;
            sample = _assets->get<cugl::Sound>("tag"+std::to_string(soundnum));
            break;
        }
        case Type::SWAP:
            sample = _assets->get<cugl::Sound>("swap");
            break;
        default:
            return;
    }
    std::shared_ptr<cugl::audio::AudioNode> node = sample->createNode();
    
    if(spatialAudioEnabled){
        std::shared_ptr<cugl::audio::AudioSpinner> spatial = cugl::audio::AudioSpinner::alloc();
        spatial->setAngle(pos.getAngle());
        spatial->attach(node);
//            cugl::AudioEngine::get()->play(key, spatial);
    }else{
        //TODO: replace with key
        cugl::AudioEngine::get()->play(std::to_string(rand()), node);
    }
    
}

}
