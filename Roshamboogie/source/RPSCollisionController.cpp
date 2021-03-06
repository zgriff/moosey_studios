//
//  RPSCollisionController.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "RPSCollisionController.h"


/** Impulse for giving collisions a slight bounce. */
#define COLLISION_COEFF     0.1f

using namespace cugl;

/**
 *  Handles collisions between ships, causing them to bounce off one another.
 *
 *  This method updates the velocities of both ships: the collider and the
 *  collidee. Therefore, you should only call this method for one of the
 *  ships, not both. Otherwise, you are processing the same collisions twice.
 *
 *  @param ship1    First ship in candidate collision
 *  @param ship2    Second ship in candidate collision
 */
void collisions::checkForCollision(const std::shared_ptr<Ship>& ship1, const std::shared_ptr<Ship>& ship2) {
    // Calculate the normal of the (possible) point of collision
    Vec2 norm = ship1->getPosition()-ship2->getPosition();
    float distance = norm.length();
    float impactDistance = (ship1->getRadius() + ship2->getRadius());
    norm.normalize();

    // If this normal is too small, there was a collision
    if (distance < impactDistance) {
        // "Roll back" time so that the ships are barely touching (e.g. point of impact).
        Vec2 temp = norm * ((impactDistance - distance) / 2);
        ship1->setPosition(ship1->getPosition()+temp);
        ship2->setPosition(ship2->getPosition()-temp);

        // Now it is time for Newton's Law of Impact.
        // Convert the two velocities into a single reference frame
        Vec2 vel = ship1->getVelocity()-ship2->getVelocity();

        // Compute the impulse (see Essential Math for Game Programmers)
        float impulse = (-(1 + COLLISION_COEFF) * norm.dot(vel)) /
                        (norm.dot(norm) * (1 / ship1->getMass() + 1 / ship2->getMass()));

        // Change velocity of the two ships using this impulse
        temp = norm * (impulse/ship1->getMass());
        ship1->setVelocity(ship1->getVelocity()+temp);

        temp = norm * (impulse/ship2->getMass());
        ship2->setVelocity(ship2->getVelocity()-temp);
    }
}

/**
 * Nudge the ship to ensure it does not do out of view.
 *
 * This code bounces the ship off walls.  You will replace it as part of
 * the lab.
 *
 * @param ship      They player's ship which may have collided
 * @param bounds    The rectangular bounds of the playing field
 */
void collisions::checkInBounds(const std::shared_ptr<Ship>& ship, const Rect bounds) {
    // UNLIKE Java, these are values, not references
//    Vec2 vel = ship->getVelocity();
    Vec2 pos = ship->getPosition();
    
    //Ensure the ship doesn't go out of view. Bounce off walls.
    if (pos.x <= bounds.origin.x) {
//        vel.x = -vel.x;
        pos.x = bounds.origin.x+bounds.size.width-1.0f;
//        ship->setVelocity(vel);
        ship->setPosition(pos);
    } else if (pos.x >= bounds.size.width+bounds.origin.x) {
//        vel.x = -vel.x;
        pos.x = bounds.origin.x;
//        ship->setVelocity(vel);
        ship->setPosition(pos);
    }

    if (pos.y <= bounds.origin.y) {
//        vel.y = -vel.y;
        pos.y = bounds.size.height+bounds.origin.y-1.0f;
//        ship->setVelocity(vel);
        ship->setPosition(pos);
    } else if (pos.y >= bounds.size.height+bounds.origin.y) {
//        vel.y = -vel.y;
        pos.y = bounds.origin.y;
//        ship->setVelocity(vel);
        ship->setPosition(pos);
    }

}
