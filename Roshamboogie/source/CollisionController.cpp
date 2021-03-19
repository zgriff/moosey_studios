//
//  CollisionController.cpp
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <stdio.h>
#include "CollisionController.h"
#include "Player.h"
#include "Egg.h"
#include "Element.h"
#include "Orb.h"
#include "SwapStation.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>

void CollisionController::beginContact(b2Contact* contact){
    b2Fixture * fixA = contact->GetFixtureA();
    b2Body * bodyA = fixA->GetBody();
    b2Fixture * fixB = contact->GetFixtureB();
    b2Body * bodyB = fixB->GetBody();
    
    cugl::physics2::Obstacle* bd1 = (cugl::physics2::Obstacle*) bodyA->GetUserData();
    cugl::physics2::Obstacle* bd2 = (cugl::physics2::Obstacle*) bodyB->GetUserData();
    
    //orb and player collision
    if(bd1->getName() == "orb" && bd2->getName() == "player") {
        Orb* o = (Orb*) bd1;
        Player* p = (Player*) bd2;
        if (o->getElement() == p->getCurrElement()) {
            o->setCollected(true);
        }
    }
    else if (bd2->getName() == "orb" && bd1->getName() == "player") {
        Orb* o = (Orb*) bd2;
        Player* p = (Player*) bd1;
        if (o->getElement() == p->getCurrElement()) {
            o->setCollected(true);
        }
    }
       
    
    //swap station and player collision
    if (bd1->getName() == "swapstation" && bd2->getName() == "player") {
        Player* p = (Player*) bd2;
        p->setElement(p->getPreyElement());
    }
    else if(bd1->getName() == "player" && bd2->getName() == "swapstation") {
        Player* p = (Player*) bd1;
        p->setElement(p->getPreyElement());
    }
    
    //egg and player collision
    if (bd1->getName() == "egg" && bd2->getName() == "player") {
        Egg* e = (Egg*) bd1;
        Player* p = (Player*) bd2;
        p->setElement(Element::None);
        e->setCollected(true);
        CULog("egg collected");
    }
    else if (bd2->getName() == "egg" && bd1->getName() == "player") {
        Egg* e = (Egg*) bd2;
        Player* p = (Player*) bd1;
        p->setElement(Element::None);
        e->setCollected(true);
        CULog("egg collected");
    }
}

void CollisionController::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold){
    
}

