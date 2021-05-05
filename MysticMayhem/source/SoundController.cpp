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
bool spatialAudioEnabled = true;
std::vector<std::string> soundKeys = {"egg", "orb1", "orb2", "orb3", "orb4", "tag1", "tag2", "tag3", "swap"};
float soundVolume = 0.5;


void useSpatialAudio(bool useSpatialAudio){
    spatialAudioEnabled = useSpatialAudio;
}

void init(std::shared_ptr<cugl::AssetManager> assets){
    _assets = assets;
    
}

void playSound(Type s, cugl::Vec2 pos){
    std::shared_ptr<cugl::Sound> sample;
    std:string key;
    switch(s){
        case Type::EGG:
            sample = _assets->get<cugl::Sound>("egg");
            key = "egg";
            break;
        case Type::ORB:
        {
            int soundnum = (rand() % NUM_ORB_SOUNDS) + 1;
            sample = _assets->get<cugl::Sound>("orb"+std::to_string(soundnum));
            key = "orb"+std::to_string(soundnum);
            break;
        }
        case Type::TAG:
        {
            int soundnum = (rand() % NUM_TAG_SOUNDS) + 1;
            sample = _assets->get<cugl::Sound>("tag"+std::to_string(soundnum));
            key = "tag"+std::to_string(soundnum);
            break;
        }
        case Type::SWAP:
            sample = _assets->get<cugl::Sound>("swap");
            key = "swap";
            break;
        default:
            return;
    }
    std::shared_ptr<cugl::audio::AudioNode> node = sample->createNode();
    
    if(spatialAudioEnabled){
        std::shared_ptr<cugl::audio::AudioSpinner> spatial = cugl::audio::AudioSpinner::alloc();
//        spatial->setChannelPlan(cugl::audio::AudioSpinner::Plan::SIDE_STEREO);
        spatial->attach(node);
//        spatial->setAngle(pos.getAngle());
        float gain = pos.length() * (4.0/50.0);
//        float gain = std::pos(pos.length() * (4.0/50.0), 2.0);
        if(gain <= 1){
            node->setGain(1);
        }else{
            node->setGain(1/gain);
        }
        //TODO: replace with key
        cugl::AudioEngine::get()->play(key, spatial, false, soundVolume);
    }else{
        //TODO: replace with key
        cugl::AudioEngine::get()->play(key, node, false, soundVolume);
    }
    
}

void setSoundVolume(float volume){
    soundVolume = volume;

}

}
