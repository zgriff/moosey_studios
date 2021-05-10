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
#include "Projectile.h"
#include "World.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "NetworkController.h"
#include "SoundController.h"
#include "Globals.h"

void CollisionController::setWorld(std::shared_ptr<World> w){
    world = w;
    localPlayer = w->getPlayer(NetworkController::getPlayerId().value());
}

void CollisionController::hostBeginContact(b2Contact* contact){
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
            SoundController::playSound(SoundController::Type::ORB, o->getPosition() - localPlayer->getPosition());
            NetworkController::sendOrbCaptured(o->getID(), p->getID());
        }
    }
          
    //swap station and player collision
    else if(bd1->getName() == "player" && bd2->getName() == "swapstation") {
        Player* p = (Player*) bd1;
        SwapStation* s = (SwapStation*) bd2;

        /*if (p->getCurrElement() != Element::None && p->getCurrElement() != Element::Aether && !p->getIsInvisible()) {
            if (s->getActive()) {
                s->setLastUsed(time(NULL));
                p->setElement(p->getPreyElement());
                s->setActive(false);
                SoundController::playSound(SoundController::Type::SWAP, s->getPosition() - localPlayer->getPosition());
                NetworkController::sendPlayerColorSwap(p->getID(), p->getCurrElement(), s->getID());
            }
        } 
        else if (p->getIsInvisible() && s->getActive()) {
            p->setElement(p->getPreyElement());
            SoundController::playSound(SoundController::Type::SWAP, s->getPosition() - localPlayer->getPosition());
        }*/

        if (p->getCurrElement() != Element::None && p->getCurrElement() != Element::Aether && s->getActive()) {
            p->setElement(p->getPreyElement());
            SoundController::playSound(SoundController::Type::SWAP, s->getPosition() - localPlayer->getPosition());
            if (!p->getIsInvisible()) {
                s->setLastUsed(time(NULL));
                s->setActive(false);
            } 
            NetworkController::sendPlayerColorSwap(p->getID(), p->getCurrElement(), s->getID());
        }
        if ((p->getIsIntangible() || p->getIsInvisible()) && p->canSwap()) {
            p->setElement(p->getPreyElement());
            SoundController::playSound(SoundController::Type::SWAP, s->getPosition() - localPlayer->getPosition());
            NetworkController::sendPlayerColorSwap(p->getID(), p->getCurrElement(), s->getID());
        }
    }
    
    //egg and player collision
    else if (bd1->getName() == "egg" && bd2->getName() == "player") {
        Egg* e = (Egg*) bd1;
        Player* p = (Player*) bd2;
        if (e->getCollected() == false && !p->getIsIntangible() && !p->getHoldingEgg()) {
            p->setElement(Element::None);
            e->setCollected(true);
            e->setPID(p->getID());
            p->setEggId(e->getID());
            p->setHoldingEgg(true);
            SoundController::playSound(SoundController::Type::EGG, e->getPosition() - localPlayer->getPosition());
            NetworkController::sendEggCollected(p->getID(), e->getID());
        }
    }

    //booster and player collision
    else if (bd1->getName() == "booster" && bd2->getName() == "player") {
        Player* p = (Player*)bd2;
        auto adjust = p->getLinearVelocity().normalize();
        p->setLinearVelocity(adjust.scale(38.0f));
    }

    //projectile and player collision (basically an ability tag)
    else if (bd1->getName() == "player" && bd2->getName() == "projectile") {
        Player* p1 = (Player*) bd1;
        Projectile* proj = (Projectile*) bd2;
        auto p2 = world->getPlayer(proj->getPlayerID());
        if (!p1->getIsIntangible() && p1 != p2.get()) {
            if (proj->getPreyElement() == Element::None || proj->getPreyElement() == p1->getCurrElement() 
                || p1->getCurrElement() == Element::None) {
                helperTag(p1, p2.get(), world, true);
            }
        }
    }
        
    //player and player collision (tagging)
    else if ((bd1->getName() == "player" && bd2->getName() == "player") || (bd2->getName() == "player" && bd1->getName() == "player")) {
        Player* p1 = (Player*) bd1;
        Player* p2 = (Player*) bd2;

        if (!p1->getIsIntangible() && !p2->getIsIntangible() && !p1->getIsTagged() && !p2->getIsTagged()) {
            //p2 tags p1
            if ((p1->getCurrElement() == p2->getPreyElement()) || (p1->getCurrElement() == Element::None 
                && p2->getCurrElement() != Element::None ) ||
                (p2->getCurrElement() == Element::Aether && p1->getCurrElement() != Element::Aether)) {
                helperTag(p1, p2, world, false);
            }
            //p1 tags p2
            else if ((p2->getCurrElement() == p1->getPreyElement()) || (p2->getCurrElement() == Element::None 
                && p1->getCurrElement() != Element::None) ||
                (p2->getCurrElement() == Element::Aether && p1->getCurrElement() != Element::Aether)) {
                helperTag(p2, p1, world, false);
            }
        }
    }
    // This is so projectiles can't be shot through walls, but we can change it.
    else if (bd1->getName() == "projectile" && bd2->getName().find("wall") != string::npos) {
        Projectile* proj = (Projectile*) bd1;
        proj->setLinearVelocity(Vec2(0, 0));
        proj->setIsGone(true);
        NetworkController::sendProjectileGone(proj->getPlayerID());
    }
}

void CollisionController::clientBeginContact(b2Contact* contact){
    b2Fixture * fixA = contact->GetFixtureA();
    b2Body * bodyA = fixA->GetBody();
    b2Fixture * fixB = contact->GetFixtureB();
    b2Body * bodyB = fixB->GetBody();
    
    cugl::physics2::Obstacle* bd1 = (cugl::physics2::Obstacle*) bodyA->GetUserData();
    cugl::physics2::Obstacle* bd2 = (cugl::physics2::Obstacle*) bodyB->GetUserData();
    if (bd1->getName() > bd2->getName()) {
        std::swap(bd1, bd2);
    }

    //orb and player collision
    if(bd1->getName() == "orb" && bd2->getName() == "player") {
        Orb* o = (Orb*) bd1;
        Player* p = (Player*) bd2;
        if (p->getIsLocal() && !o->getCollected() && p->getCurrElement() != Element::None && p->getIsIntangible() == false) {
            o->setCollected(true);
        }
    }
    //booster and player collision
    else if (bd1->getName() == "booster" && bd2->getName() == "player") {
        Player* p = (Player*)bd2;
        auto adjust = p->getLinearVelocity();
        p->setLinearVelocity(adjust.scale(45.0f / adjust.length()));
    }
}

void CollisionController::helperTag(Player* tagged, Player* tagger, std::shared_ptr<World> world, 
                                        bool dropEgg) {
    tagged->setIsTagged(true);
    time_t timestamp = time(NULL);
    tagged->setTimeLastTagged(timestamp);
    tagger->incScore(globals::TAG_SCORE);
    tagger->animateTag();
    SoundController::playSound(SoundController::Type::TAG, tagger->getPosition() - localPlayer->getPosition());
    NetworkController::sendTag(tagged->getID(), tagger->getID(), timestamp, dropEgg);
    if (tagged->getCurrElement() == Element::None) {
        auto egg = world->getEgg(tagged->getEggId());
        tagged->setElement(tagged->getPrevElement());
        egg->setDistanceWalked(0);
        if (!dropEgg) {
            //p1 holding egg and p2 steals it
            egg->setPID(tagger->getID());
            tagger->setElement(Element::None);
            tagger->setEggId(egg->getID());
            tagged->setHoldingEgg(false);
            tagger->setHoldingEgg(true);
        }
        else {
            //tagged hit by projectile so drops the egg
            egg->setCollected(false);
            egg->setInitPos(tagged->getPosition()); //this is because player tagged remains in same location and doens't respawn
            tagged->setJustHitByProjectile(true);
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


