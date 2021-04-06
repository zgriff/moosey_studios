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
#include "World.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "NetworkController.h"

void CollisionController::setWorld(std::shared_ptr<World> w){
    world = w;
}

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
        if (!o->getCollected() && p->getCurrElement() != Element::None && p->getIsIntangible() == false) {
            o->setCollected(true);
            p->incScore(1);
            o->dispose();
            world->setOrbCount(world->getCurrOrbCount() - 1);
            NetworkController::sendOrbCaptured(o->getID(), p->getID());
        }
    }
    else if (bd2->getName() == "orb" && bd1->getName() == "player") {
        Orb* o = (Orb*) bd2;
        Player* p = (Player*) bd1;
        if (!o->getCollected() && p->getCurrElement() != Element::None && p->getIsIntangible() == false) {
            o->setCollected(true);
            p->incScore(1);
            o->dispose();
            world->setOrbCount(world->getCurrOrbCount() - 1);
            NetworkController::sendOrbCaptured(o->getID(), p->getID());
        }
    }
       
    //swap station and player collision
   else if (bd1->getName() == "swapstation" && bd2->getName() == "player") {
        Player* p = (Player*) bd2;
        SwapStation* s = (SwapStation*) bd1;
        if (p->getCurrElement() != Element::None && p->getIsIntangible() == false) {
            if (time(NULL) - s->getLastUsed() >= s->getCoolDown()) {
                s->setLastUsed(time(NULL));
                p->setElement(p->getPreyElement());
    //            s->setActive(false);
                NetworkController::sendPlayerColorSwap(p->getID(), p->getCurrElement(), s->getID());
            }
        }
    }
    else if(bd1->getName() == "player" && bd2->getName() == "swapstation") {
        Player* p = (Player*) bd1;
        SwapStation* s = (SwapStation*) bd2;
        if (p->getCurrElement() != Element::None && p->getIsIntangible() == false) {
            if (time(NULL) - s->getLastUsed() >= s->getCoolDown()) {
                s->setLastUsed(time(NULL));
                p->setElement(p->getPreyElement());
    //            s->setActive(false);
                NetworkController::sendPlayerColorSwap(p->getID(), p->getCurrElement(), s->getID());
            }
        }
    }
    
    //egg and player collision
    else if (bd1->getName() == "egg" && bd2->getName() == "player") {
        Egg* e = (Egg*) bd1;
        Player* p = (Player*) bd2;
        if (e->getCollected() == false && p->getIsIntangible() == false) {
            p->setElement(Element::None);
            e->setCollected(true);
            e->setPID(p->getID());
            CULog("egg collected");
            NetworkController::sendEggCollected(p->getID(), e->getID());
        }
        
    }
    else if (bd2->getName() == "egg" && bd1->getName() == "player") {
        Egg* e = (Egg*) bd2;
        Player* p = (Player*) bd1;
        if (e->getCollected() == false && p->getIsIntangible() == false) {
            p->setElement(Element::None);
            e->setCollected(true);
            e->setPID(p->getID());
            CULog("egg collected");
            NetworkController::sendEggCollected(p->getID(), e->getID());
        }
    }
    
    //player and player collision (tagging)
    else if ((bd1->getName() == "player" && bd2->getName() == "player") || (bd2->getName() == "player" && bd1->getName() == "player")) {
        Player* p1 = (Player*) bd1;
        Player* p2 = (Player*) bd2;

        if (p1->getIsIntangible() == false && p2->getIsIntangible() == false) {
            //p2 tags p1
            if (p1->getCurrElement() == p2->getPreyElement()) {
                CULog("tagged");
                p1->setIsTagged(true);
                time_t timestamp = time(NULL);
                p1->setTagCooldown(timestamp);
                p2->setDidTag(true);
                NetworkController::sendTag(p1->getID(), p2->getID(), timestamp);
            }
            //p1 tags p2
            else if (p2->getCurrElement() == p1->getPreyElement()) {
                CULog("tagged");
                p2->setIsTagged(true);
                time_t timestamp = time(NULL);
                p2->setTagCooldown(timestamp);
                p1->setDidTag(true);
                NetworkController::sendTag(p2->getID(), p1->getID(), timestamp);
            }
        }
    }
}


void CollisionController::endContact(b2Contact* contact) {
    b2Fixture * fixA = contact->GetFixtureA();
    b2Body * bodyA = fixA->GetBody();
    b2Fixture * fixB = contact->GetFixtureB();
    b2Body * bodyB = fixB->GetBody();
    
    cugl::physics2::Obstacle* bd1 = (cugl::physics2::Obstacle*) bodyA->GetUserData();
    cugl::physics2::Obstacle* bd2 = (cugl::physics2::Obstacle*) bodyB->GetUserData();
    
    if ((bd1->getName() == "player" && bd2->getName() == "player") || (bd2->getName() == "player" && bd1->getName() == "player")) {
        Player* p1 = (Player*) bd1;
        Player* p2 = (Player*) bd2;
//        p1->setIsTagged(false);
//        p2->setIsTagged(false);
        p1->setDidTag(false);
        p2->setDidTag(false);
    }
}

void CollisionController::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold){

    b2Fixture * fixA = contact->GetFixtureA();
    b2Fixture * fixB = contact->GetFixtureB();
    b2Body * bodyA = fixA->GetBody();
    b2Body * bodyB = fixB->GetBody();

    cugl::physics2::Obstacle* bd1 = (cugl::physics2::Obstacle*) bodyA->GetUserData();
    cugl::physics2::Obstacle* bd2 = (cugl::physics2::Obstacle*) bodyB->GetUserData();

    //disable two player collision (pass through each other)
    if ((bd1->getName() == "player" && bd2->getName() == "player") || (bd2->getName() == "player" && bd1->getName() == "player")) {
        contact->SetEnabled(false);
    }
}


