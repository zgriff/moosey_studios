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
#include "Booster.h"
#include "SwapStation.h"
#include "World.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "NetworkController.h"
#include "Globals.h"

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
    if (bd1->getName() > bd2->getName()) {
        std::swap(bd1, bd2);
    }
    //object that comes first lexograpically

    //orb and player collision
    if(bd1->getName() == "orb" && bd2->getName() == "player") {
        Orb* o = (Orb*) bd1;
        Player* p = (Player*) bd2;
        if (!o->getCollected() && p->getCurrElement() != Element::None && p->getIsIntangible() == false) {
            world->addOrbSpawn(o->getPosition());
            o->setCollected(true);
            p->setOrbScore(p->getOrbScore() + 1);
            world->setOrbCount(world->getCurrOrbCount() - 1);
            NetworkController::sendOrbCaptured(o->getID(), p->getID());
        }
    }
          
    //swap station and player collision
    else if(bd1->getName() == "player" && bd2->getName() == "swapstation") {
        Player* p = (Player*) bd1;
        SwapStation* s = (SwapStation*) bd2;

        if (p->getCurrElement() != Element::None && p->getCurrElement() != Element::Aether && p->getIsIntangible() == false) {
            if (s->getActive()) {
                s->setLastUsed(time(NULL));
                p->setElement(p->getPreyElement());
                s->setActive(false);
                NetworkController::sendPlayerColorSwap(p->getID(), p->getCurrElement(), s->getID());
            }
        }
    }
    
    //egg and player collision
    else if (bd1->getName() == "egg" && bd2->getName() == "player") {
        Egg* e = (Egg*) bd1;
        Player* p = (Player*) bd2;
        if (e->getCollected() == false && p->getIsIntangible() == false && !p->getHoldingEgg()) {
            p->setElement(Element::None);
            e->setCollected(true);
            e->setPID(p->getID());
            p->setEggId(e->getID());
            p->setHoldingEgg(true);
            CULog("egg collected");
            NetworkController::sendEggCollected(p->getID(), e->getID());
        }
    }

    //booster and player collision
    else if (bd1->getName() == "booster" && bd2->getName() == "player") {
        Player* p = (Player*)bd2;
        auto adjust = p->getLinearVelocity();
        p->setLinearVelocity(adjust.scale(45.0f / adjust.length()));
    }
        
    //player and player collision (tagging)
    else if ((bd1->getName() == "player" && bd2->getName() == "player") || (bd2->getName() == "player" && bd1->getName() == "player")) {
        Player* p1 = (Player*) bd1;
        Player* p2 = (Player*) bd2;

        if (p1->getIsTagged() == false && p2->getIsTagged() == false && p1->getIsIntangible() == false && p2->getIsIntangible() == false) {
            //p2 tags p1
            if ((p1->getCurrElement() == p2->getPreyElement()) || (p1->getCurrElement() == Element::None 
                && p2->getCurrElement() != Element::None ) ||
                (p2->getCurrElement() == Element::Aether && p1->getCurrElement() != Element::Aether)) {
                CULog("tagged");
                p1->setIsTagged(true);
                time_t timestamp = time(NULL);
                p1->setTagCooldown(timestamp);
                p2->incScore(globals::TAG_SCORE);
                NetworkController::sendTag(p1->getID(), p2->getID(), timestamp);
                //p1 holding egg and p2 steals it
                if (p1->getCurrElement() == Element::None) {
                    auto egg = world->getEgg(p1->getEggId());
                    egg->setPID(p2->getID());
                    p1->setElement(p1->getPrevElement());
                    egg->setDistanceWalked(0);
                    p2->setElement(Element::None);
                    p2->setEggId(egg->getID());
                    p1->setHoldingEgg(false);
                    p2->setHoldingEgg(true);
                }
            }
            //p1 tags p2
            else if ((p2->getCurrElement() == p1->getPreyElement()) || (p2->getCurrElement() == Element::None 
                && p1->getCurrElement() != Element::None) ||
                (p2->getCurrElement() == Element::Aether && p1->getCurrElement() != Element::Aether)) {
                CULog("tagged");
                p2->setIsTagged(true);
                time_t timestamp = time(NULL);
                p2->setTagCooldown(timestamp);
                p1->incScore(globals::TAG_SCORE);
                NetworkController::sendTag(p2->getID(), p1->getID(), timestamp);
                //p2 holding egg and p1 steals it
                if (p2->getCurrElement() == Element::None) {
                    auto egg = world->getEgg(p2->getEggId());
                    egg->setPID(p1->getID());
                    p2->setElement(p2->getPrevElement());
                    egg->setDistanceWalked(0);
                    p1->setElement(Element::None);
                    p1->setEggId(egg->getID());
                    p2->setHoldingEgg(false);
                    p1->setHoldingEgg(true);
                }
            }
        }
    }
}


void CollisionController::endContact(b2Contact* contact) {
//    b2Fixture * fixA = contact->GetFixtureA();
//    b2Body * bodyA = fixA->GetBody();
//    b2Fixture * fixB = contact->GetFixtureB();
//    b2Body * bodyB = fixB->GetBody();
//    
//    cugl::physics2::Obstacle* bd1 = (cugl::physics2::Obstacle*) bodyA->GetUserData();
//    cugl::physics2::Obstacle* bd2 = (cugl::physics2::Obstacle*) bodyB->GetUserData();
//    
//    if ((bd1->getName() == "player" && bd2->getName() == "player") || (bd2->getName() == "player" && bd1->getName() == "player")) {
//        Player* p1 = (Player*) bd1;
//        Player* p2 = (Player*) bd2;
////        p1->setIsTagged(false);
////        p2->setIsTagged(false);
//        p1->setDidTag(false);
//        p2->setDidTag(false);
//    }
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


