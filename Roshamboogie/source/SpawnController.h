//
//  SpawnController.h
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/30/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef SpawnController_h
#define SpawnController_h

#include <cugl/cugl.h>
#include "World.h"

namespace SpawnController {

namespace {
    std::shared_ptr<World> world;
    std::random_device r;
    std::default_random_engine e1(r());
}

void setWorld(std::shared_ptr<World> w);

void spawnOrbs();

void spawnEggs();
}

#endif /* SpawnController_h */
