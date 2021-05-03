//
//  CollisionController.h
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef CollisionController_h
#define CollisionController_h

#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "World.h"

#include <cugl/cugl.h>


using namespace cugl;

namespace CollisionController {

    namespace {
        std::shared_ptr<World> world;
        std::shared_ptr<Player> localPlayer;
    }

    void hostBeginContact(b2Contact* contact);
    void clientBeginContact(b2Contact* contact);

    void endContact(b2Contact* contact);
    
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);

    void setWorld(std::shared_ptr<World> w);

    void helperTag(Player* tagged, Player* tagger, std::shared_ptr<World> world, bool dropEgg);
};


#endif /* CollisionController_h */
