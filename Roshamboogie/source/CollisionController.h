//
//  CollisionController.hpp
//  Roshamboogie
//
//  Created by Joshua Kaplan on 3/7/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef CollisionController_h
#define CollisionController_h

#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>

namespace collisions {
    void beginContact(b2Contact* contact);
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);
}

#endif /* CollisionController_h */
