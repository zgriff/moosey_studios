//
//  RPSCollisionController.hpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __RPS_COLLISION_CONTROLLER_H__
#define __RPS_COLLISION_CONTROLLER_H__
#include "RPSShip.h"
#include <vector>
#include <cugl/cugl.h>
#include <stdio.h>

namespace collisions {
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
void checkForCollision(const std::shared_ptr<Ship>& ship1, const std::shared_ptr<Ship>& ship2);


/**
 * Nudge the ship to ensure it does not do out of view.
 *
 * This code bounces the ship off walls.  You will replace it as part of
 * the lab.
 *
 * @param ship      They player's ship which may have collided
 * @param bounds    The rectangular bounds of the playing field
 */
void checkInBounds(const std::shared_ptr<Ship>& ship, const cugl::Rect bounds);



}

#endif /* RPSCollisionController_hpp */
