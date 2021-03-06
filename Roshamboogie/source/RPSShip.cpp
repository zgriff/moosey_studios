//
//  RPSPlayer.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "RPSShip.h"

/** The size of the ship in pixels (image is square) */
#define SHIP_SIZE       81
/** Number of rows in the ship image filmstrip */
#define SHIP_ROWS       4
/** Number of columns in this ship image filmstrip */
#define SHIP_COLS       5
/** Number of elements in this ship image filmstrip */
#define SHIP_FRAMES     18
/** The frame number for the tightest bank for a left turn */
#define SHIP_IMG_LEFT   0
/** The frame number for a ship that is not turning */
#define SHIP_IMG_FLAT   9
/** The frame number for the tightest bank for a right turn */
#define SHIP_IMG_RIGHT  17
/** The amount to offset the shadow image by */
#define SHADOW_OFFSET   10.0f
/** The size of the target reticule in pixels (image is square) */
#define TARGET_SCALE    0.75
/** Distance from ship to target reticule */
#define TARGET_DIST     100
/** Amount to adjust forward movement from input */
#define THRUST_FACTOR   0.4f
/** Amount to adjust angular movement from input */
#define BANK_FACTOR     0.1f
/** Maximum turning/banking speed */
#define MAXIMUM_BANK    5.0f
/** Amount to decay forward thrust over time */
#define FORWARD_DAMPING 0.9f
/** Amount to angular movement over time */
#define ANGULAR_DAMPING 0.875f


using namespace cugl;

/**
 * Sets the textures for this ship.
 *
 * The two textures are the ship texture and the target texture. The
 * scene graph node associated with this ship is nullptr until these
 * values are set.
 *
 * @param ship      The texture for the ship filmstrip
 * @param target    The texture for the ship target
 */
void Ship::setTextures(const std::shared_ptr<Texture>& ship,
                       float width, float height) {

    _sceneNode = scene2::SceneNode::allocWithPosition(_pos);
    _shipNode = scene2::AnimationNode::alloc(ship, SHIP_ROWS, SHIP_COLS, SHIP_FRAMES);
    _shipNode->setAnchor(Vec2::ANCHOR_CENTER);
    _shipNode->setFrame(SHIP_IMG_FLAT);
    _shipNode->setAngle(M_PI*_ang/180.0f);
    _shipNode->setPosition(0,0);
    _shipNode->setColor(_tint);
    
    _sceneNode->addChild(_shipNode);
}

/**
 * Sets the position of this ship.
 *
 * This is location of the center pixel of the ship on the screen.
 *
 * @param value the position of this ship
 */
void Ship::setPosition(cugl::Vec2 value) {
    _pos = value;
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(_pos);
    }
}

/**
 * Sets the angle that this ship is facing.
 *
 * The angle is specified in degrees, not radians.
 *
 * @param value the angle of the ship
 */
void Ship::setAngle(float value) {
    _ang = value;
    if (_sceneNode != nullptr) {
        _shipNode->setAngle(M_PI*_ang/180.0f);
    }
}


/**
 * Sets the tint color for this ship.
 *
 * We can change how an image looks without loading a new image by
 * tinting it differently.
 *
 * @param value the tint color
 */
void Ship::setColor(Color4f value) {
    _tint = value;
    if (_shipNode != nullptr) {
        _shipNode->setColor(_tint);
    }
}

/**
 * Creates a ship with the default values.
 *
 * To properly initialize the ship, you should call the init
 * method.
 */
Ship::Ship() :
_radius(SHIP_SIZE/2),
_mass(1.0f),
_ang(0),
_dang(0)
{
    _tint = Color4f::WHITE;
    _stint = Color4f(0,0,0,0.5);
}

/**
 * Disposes the ship, releasing all resources.
 */
void Ship::dispose() {
    // Garbage collect
    _sceneNode = nullptr;
    _shipNode = nullptr;
    _shipTexture = nullptr;
    // Restore to defaults
    _tint = Color4f::WHITE;
    _stint = Color4f(0,0,0,0.5);
    _mass = 1.0f;
    _ang = 0;
    _dang = 0;
    _radius = SHIP_SIZE/2;
    _pos = Vec2::ZERO;
    _vel = Vec2::ZERO;
}

/**
 * Initializes a new ship at the given location with the given facing.
 *
 * This method does NOT create a scene graph node for this ship.  You
 * must call setTextures for that.
 *
 * @param x The initial x-coordinate of the center
 * @param y The initial y-coordinate of the center
 * @param ang The initial angle of rotation
 *
 * @return true if the initialization was successful
 */
bool Ship::init(float x, float y, float ang) {
    _pos.set(x,y);
    _ang = ang;
    return true;
}

#pragma mark Movement
/**
 * Moves the ship by the specified amount.
 *
 * Forward is the amount to move forward, while turn is the angle to turn the ship
 * (used for the "banking" animation. This method performs no collision detection.
 * Collisions are resolved afterwards.
 *
 * @param forward    Amount to move forward
 * @param turn        Amount to turn the ship
 */
void Ship::move(float forward, float turn) {
    // Process the ship turning.
    processTurn(turn);

    // Process the ship thrust.
    if (forward != 0.0f) {
        // Thrust key pressed; increase the ship velocity.
        float rads = M_PI*_ang/180.0f+M_PI_2;
        Vec2 dir(cosf(rads),sinf(rads));
        _vel += dir * forward * THRUST_FACTOR;
    } else {
        // Gradually slow the ship down
        _vel *= FORWARD_DAMPING;
    }

    // Move the ship, updating it.
    // Adjust the angle by the change in angle
    setAngle(_ang+_dang);
    
    // INVARIANT: -360 < ang < 720
    if (_ang > 360)
        _ang -= 360;
    if (_ang < 0)
        _ang += 360;
    
    
    // Move the ship position by the ship velocity
    // The setter also updates the scene graph
    setPosition(_pos+_vel);

}

/**
 * Update the animation of the ship to process a turn
 *
 * Turning changes the frame of the filmstrip, as we change from a level ship to
 * a hard bank.  This method also updates the field dang cumulatively.
 *
 * @param turn Amount to turn the ship
 */
void Ship::processTurn(float turn) {
    int frame = (_shipNode == nullptr ? 0 : _shipNode->getFrame());
    if (turn != 0.0f) {
        // The turning factor is cumulative.
        // The longer it is held down, the harder we bank.
        _dang -= turn/BANK_FACTOR;
        if (_dang < -MAXIMUM_BANK) {
            _dang = -MAXIMUM_BANK;
        } else if (_dang > MAXIMUM_BANK) {
            _dang = MAXIMUM_BANK;
        }

        // SHIP_IMG_RIGHT represents the hardest bank possible.
        if (turn > 0 && frame < SHIP_IMG_RIGHT) {
            frame++;
        } else if (turn < 0 && frame > SHIP_IMG_LEFT) {
            frame--;
        }
    } else {
        // If neither key is pressed, slowly flatten out ship.
        if (_dang != 0) {
            _dang *= ANGULAR_DAMPING;   // Damping factor.
        }
        if (frame < SHIP_IMG_FLAT) {
            frame++;
        } else if (frame > SHIP_IMG_FLAT) {
            frame--;
        }
    }
    if (_shipNode != nullptr) {
        _shipNode->setFrame(frame);
    }
}
