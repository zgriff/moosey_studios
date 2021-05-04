//
//  Player.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "Player.h"
#include "Element.h"
#include "Projectile.h"
#include "NetworkController.h"




using namespace cugl;

#pragma mark -
#pragma mark Physics Constants

// Default physics values
/** The density of the player */
#define DEFAULT_DENSITY 1.0f
/** The friction of the player */
#define DEFAULT_FRICTION 0.0f
/** The minimum total velocity for drag to apply */
#define THRESHOLD_VELOCITY 27.0f
/** What proportion of the player's extra velocity is lost per frame*/
#define SPEEDING_DRAG 0.006f

/** The restitution of this rocket */
#define DEFAULT_RESTITUTION 0.4f
/** The constant force applied to this rocket */
#define DEFAULT_PLAYER_FORCE Vec2(0.0f, 8.3f)
/** Number of rows in the player image filmstrip */
#define PLAYER_ROWS       1
/** Number of columns in this player image filmstrip */
#define PLAYER_COLS       8
/** Number of elements in this player image filmstrip */
#define PLAYER_FRAMES     8
/** How fast a player recovers from screen shake*/
#define TRAUMA_RECOVERY   .005f
/** How fast a player moves to physics body location. Between 0 and 1 */
#define INTERPOLATION_AMOUNT 0.9f

#pragma mark -
#pragma mark Animation Constants

#define SKIN_ROWS           1
#define SKIN_COLS           8
#define SKIN_FRAMES         8

#define COLOR_ROWS          1
#define COLOR_COLS          20
#define COLOR_FRAMES        20

#define FACE_ROWS           1
#define FACE_COLS           8
#define FACE_FRAMES         8

#define BODY_ROWS           1
#define BODY_COLS           4
#define BODY_FRAMES         4

#define HAT_ROWS            1
#define HAT_COLS            12
#define HAT_FRAMES          12

#define STAFF_ROWS          1
#define STAFF_COLS          24
#define STAFF_FRAMES        24

#define RING_ROWS           1
#define RING_COLS           6
#define RING_FRAMES         6

#define PLAYER_ANIM_FRAMES  4

#define WALKING_VELOCITY    0.5f
#define RUNNING_VELOCITY    7.0F

/**
 * Sets the textures for this player.
 *
 * @param player      The texture for the player filmstrip
 */
void Player::setTextures(const std::shared_ptr<AssetManager>& assets) {

    _sceneNode = scene2::PolygonNode::alloc();
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _texture = assets->get<Texture>("player");
    
    _animationNode = scene2::AnimationNode::alloc(_texture, PLAYER_ROWS, PLAYER_COLS, PLAYER_FRAMES);
    
    std::shared_ptr<scene2::AnimationNode>  skinNode = scene2::AnimationNode::alloc(assets->get<Texture>(_skinKey), SKIN_ROWS, SKIN_COLS, SKIN_FRAMES);
    std::shared_ptr<scene2::AnimationNode>  colorNode = scene2::AnimationNode::alloc(assets->get<Texture>(_colorKey), COLOR_ROWS, COLOR_COLS, COLOR_FRAMES);
    std::shared_ptr<scene2::AnimationNode>  faceNode = scene2::AnimationNode::alloc(assets->get<Texture>(_faceKey), FACE_ROWS, FACE_COLS, FACE_FRAMES);
    std::shared_ptr<scene2::AnimationNode>  bodyNode = scene2::AnimationNode::alloc(assets->get<Texture>(_bodyKey), BODY_ROWS, BODY_COLS, BODY_FRAMES);
    std::shared_ptr<scene2::AnimationNode>  hatNode = scene2::AnimationNode::alloc(assets->get<Texture>(_hatKey), HAT_ROWS, HAT_COLS, HAT_FRAMES);
    std::shared_ptr<scene2::AnimationNode>  staffNode = scene2::AnimationNode::alloc(assets->get<Texture>(_staffKey), STAFF_ROWS, STAFF_COLS, STAFF_FRAMES);
    std::shared_ptr<scene2::AnimationNode>  ringNode = scene2::AnimationNode::alloc(assets->get<Texture>(_ringKey), RING_ROWS, RING_COLS, RING_FRAMES);
    
    _animNodes[_skinKey] = skinNode;
    _animNodes[_colorKey] = colorNode;
    _animNodes[_faceKey] = faceNode;
    _animNodes[_bodyKey] = bodyNode;
    _animNodes[_hatKey] = hatNode;
    _animNodes[_staffKey] = staffNode;
    _animNodes[_ringKey] = ringNode;
    
    //TODO: fix hat animation
    _animNodes[_hatKey]->setFrame(1);
    
    _frameNumbers[_skinKey] = 4;
    _frameNumbers[_colorKey] = 4;
    _frameNumbers[_faceKey] = 4;
    _frameNumbers[_bodyKey] = 4;
    _frameNumbers[_hatKey] = 1;
    _frameNumbers[_staffKey] = 4;
    _frameNumbers[_ringKey] = 2;
    
    for (auto it = _animNodes.begin(); it !=  _animNodes.end(); ++it) {
        (*it).second->setAnchor(Vec2::ANCHOR_CENTER);
        (*it).second->setPosition(0,0);
        _animCycles[(*it).first] = true;
    }
    
    
    _sceneNode->addChild(skinNode);
    _sceneNode->addChild(colorNode);
    _sceneNode->addChild(faceNode);
    _sceneNode->addChild(bodyNode);
    _sceneNode->addChild(hatNode);
    _sceneNode->addChild(staffNode);
    _sceneNode->addChild(ringNode);
    
    
    _sceneNode->setScale(0.1f);
    
    _animationTimer = time(NULL);
    
    setElement(_currElt);

    _body->SetUserData(this);

}

void Player::setElement(Element e){
    // this is so if you collect the egg as Aether, it will revert back to your orignal color
    // since keeping track that prev element is Aether doesn't seem to have any use atm
    if (_currElt != Element::Aether) {
        _prevElt = _currElt;
    }
    _currElt = e;
    
    //Need to flip before and after setting frame bc bug with texture on polygon
    //Staff asset faces opposite direction
    if (_horizFlip) {
        _animNodes[_colorKey]->flipHorizontal(false);
    } else {
        _animNodes[_staffKey]->flipHorizontal(false);
    }
    
    switch(e){
        case Element::Grass:
            _animNodes[_colorKey]->setFrame(calculateFrame(8,_colorKey));
            _animNodes[_staffKey]->setFrame(calculateFrame(8,_staffKey));
            _animNodes[_ringKey]->setFrame(calculateFrame(4,_ringKey));
            break;
        case Element::Fire:
            _animNodes[_colorKey]->setFrame(calculateFrame(0,_colorKey));
            _animNodes[_staffKey]->setFrame(calculateFrame(0,_staffKey));
            _animNodes[_ringKey]->setFrame(calculateFrame(0,_ringKey));
            break;
        case Element::Water:
            _animNodes[_colorKey]->setFrame(calculateFrame(4,_colorKey));
            _animNodes[_staffKey]->setFrame(calculateFrame(4,_staffKey));
            _animNodes[_ringKey]->setFrame(calculateFrame(2,_ringKey));
            break;
        case Element::None:
            _animNodes[_colorKey]->setFrame(calculateFrame(16,_colorKey));
            _animNodes[_staffKey]->setFrame(calculateFrame(16,_staffKey));
            break;
        case Element::Aether:
            _animNodes[_colorKey]->setFrame(calculateFrame(12,_colorKey));
            _animNodes[_staffKey]->setFrame(calculateFrame(12,_staffKey));
            break;
    }
    
    if (_horizFlip) {
        _animNodes[_colorKey]->flipHorizontal(true);
    } else  {
        _animNodes[_staffKey]->flipHorizontal(true);
    }
}

Element Player::getPreyElement() {
    switch(_currElt){
        case Element::Grass:
            return Element::Water;
        case Element::Fire:
            return Element::Grass;
        case Element::Water:
            return Element::Fire;
        case Element::None:
            return Element::None;
        case Element::Aether:
            //All three elements are prey, so this gets handled seperately in collision controller
            return Element::None;
    }
}

void Player::allocUsernameNode(const std::shared_ptr<cugl::Font>& font) {
    _usernameNode = scene2::Label::alloc(_username, font);
    _usernameNode->setPosition(-1*_usernameNode->getContentWidth()/2, 40);
    /*Hardcoded height because not sure how to get dimensions of the Player
    CULog("this width %d", this->getWidth());
    CULog("sceneNode width %d", _sceneNode->getContentWidth());
    CULog("animationNode width %d", _animationNode->getContentWidth());*/
    _sceneNode->addChild(_usernameNode);
}

/**
 * Disposes the player, releasing all resources.
 */
void Player::dispose() {
    // Garbage collect
    _sceneNode = nullptr;
    _animationNode = nullptr;
    _texture = nullptr;
    _animNodes.clear();
    _animCycles.clear();
    _frameNumbers.clear();
    _skinKey = "";
    _colorKey = "";
    _faceKey = "";
    _bodyKey = "";
    _hatKey = "";
    _staffKey = "";
    _ringKey = "";
    _projectile = nullptr;
}

/**
 * Initializes a new player at the given location with the given facing.
 *
 * This method does NOT create a scene graph node for this player.  You
 * must call setTextures for that.
 *
 * @param x The initial x-coordinate of the center
 * @param y The initial y-coordinate of the center
 * @param ang The initial angle of rotation
 *
 * @return true if the initialization was successful
 */
bool Player::init(const cugl::Vec2 pos, const cugl::Size size, Element elt) {
    if(physics2::CapsuleObstacle::init(pos,size)){
        std::string name("player");
        setName(name);
        setDensity(DEFAULT_DENSITY);
        setFriction(DEFAULT_FRICTION);
        setRestitution(DEFAULT_RESTITUTION);
        setFixedRotation(true);
        setForce(DEFAULT_PLAYER_FORCE);
        _currElt = elt;
        _prevElt = elt;
        _score = 0;
        _tagCooldown = 0;
        _isTagged = false;
        _didTag = false;
        _positionError = Vec2::ZERO;
        _holdingEgg = false;
        return true;
    }
    return false;
}

/** sets the players direction
 *  don't use values outside of the range (-2 * PI, 4 * PI)
 */
void Player::setDirection(float d) {
    _direct = d > 2.0 * M_PI ? d - 2.0 * M_PI : (d < 0.0 ? d + 2.0 * M_PI : d);
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * This method is called AFTER the collision resolution state. Therefore, it
 * should not be used to process actions or any other gameplay information.
 * Its primary purpose is to adjust changes to the fixture, which have to
 * take place after collision.
 *
 * In other words, this is the method that updates the scene graph.  If you
 * forget to call it, it will not draw your changes.
 *
 * @param delta Timing values from parent loop
 */
void Player::update(float delta) {
    Obstacle::update(delta);
    if (_sceneNode != nullptr) {
        //interp
//        auto dest = (getPosition()*_drawscale) + _positionError;
//        CULog("%s\n", _positionError.toString().c_str());
//        _positionError *= INTERPOLATION_AMOUNT;
        if(_positionError.length() < 0.00001f){
            _positionError.setZero();
        }
        _sceneNode->setPosition((getPosition() + _positionError) * _drawscale);
//        _sceneNode->setAngle(getAngle());
        
        _positionError *= INTERPOLATION_AMOUNT;
    }

    _trauma = max(0.0f, _trauma - TRAUMA_RECOVERY);

    if (getLinearVelocity().length() > THRESHOLD_VELOCITY) {
        auto adjust = getLinearVelocity();
        adjust.normalize().scale(1.0f * getMass() * SPEEDING_DRAG * (adjust.length() - THRESHOLD_VELOCITY));
        getBody()->ApplyLinearImpulseToCenter(b2Vec2(adjust.x, adjust.y), true);
    }
    
    if (_isTagged) {
        _isInvisible = true;
        _isIntangible = true;
        _sceneNode->setColor(Color4(255,255,255,50));
    }
    else {
        _isInvisible = false;
        _isIntangible = false;
        if (_currElt != Element::Aether) {
            _sceneNode->setColor(Color4(255, 255, 255, 255));
        }
    }
    
    if (_isInvisible) {
        //set invisible for other players
        if(! _isLocal){
            _sceneNode->setVisible(false);
        }
    } else {
        _sceneNode->setVisible(true);
    }
}

void Player::addTrauma(float t) {
    _trauma = min(1.0f, _trauma + t);
}

void Player::applyForce() {
    if (!isActive()) {
        return;
    }
    
    // Orient the force with rotation.
    Vec4 netforce(_force.x,_force.y,0.0f,1.0f);
    Mat4::createRotationZ(getDirection(), &_affine);
    netforce *= _affine;
    
    // Apply force to the rocket BODY, not the rocket
    _body->ApplyForceToCenter(b2Vec2(netforce.x,netforce.y), true);
}

void Player::setDrawScale(float scale) {
    _drawscale = scale;
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition()*_drawscale);
    }
}

void Player::animateMovement() {
    //TODO: change this to be more elegant
    //Rotates directional ring around player, offset by orientation in png
    if (getLinearVelocity().length() < WALKING_VELOCITY)  {
        _animationRate = 1.5f * CLOCKS_PER_SEC;
    } else if (getLinearVelocity().length() < RUNNING_VELOCITY) {
        _animationRate = 0.5f * CLOCKS_PER_SEC;
    } else {
        _animationRate = 0.075f * CLOCKS_PER_SEC;
    }
    
    
    if (_animNodes[_ringKey] != nullptr) {
        _animNodes[_ringKey]->setAngle(getDirection()-.25*M_PI);
    }
    
    //Check to see if player is going right or left and flip sprite
    if (getDirection() >= M_PI)  {
        flipHorizontal(true);
        _horizFlip = true;
    } else if (getDirection() < M_PI) {
        flipHorizontal(false);
        _horizFlip = false;
    }
    
    //iterate through nodes and animate at animation rate
    if (clock() - _animationTimer  >= _animationRate) {
        for (auto it = _animNodes.begin(); it !=  _animNodes.end(); ++it) {
            animationCycle((*it).second.get(), &_animCycles[(*it).first], (*it).first);
        }
        _animationTimer = clock();
    }
}


void Player::animationCycle(scene2::AnimationNode* node, bool* cycle, std::string key) {
    if (node->getFrame()%_frameNumbers[key] == _frameNumbers[key]-1 && !node->isFlipHorizontal()) {
        *cycle = false;
    } else if (node->getFrame()%_frameNumbers[key] == 0 && node->isFlipHorizontal()) {
        *cycle = false;
    } else {
        *cycle = true;
    }
    
//    CULog("frame:   %i", node->getFrame());
    //Need to flip before and after setting frame bc bug with texture on polygon
    bool flip = node->isFlipHorizontal();
    if (flip) {
        node->flipHorizontal(false);
    }
    
    // Increment
    if (*cycle) {
        if (flip) {
            node->setFrame(node->getFrame()-1);
        } else {
            node->setFrame(node->getFrame()+1);
        }
    } else {
        //Return to beginning of animation cycle
        if (flip) {
            node->setFrame(node->getFrame()+(_frameNumbers[key]-1));
        } else {
            node->setFrame(node->getFrame()-(_frameNumbers[key]-1));
        }
        
    }
    
    if (flip) {
        node->flipHorizontal(true);
    }
}

//iterate through animation nodes and flip if not ring
void Player::flipHorizontal(bool flip) {
    for (auto it = _animNodes.begin(); it !=  _animNodes.end(); ++it) {
        //staff asset faces the opposite direction as all other nodes
        if ((*it).first == _staffKey) {
            if ((*it).second->isFlipHorizontal()==flip) {
                (*it).second->setFrame((*it).second->getSize()-(*it).second->getFrame()-1);
                (*it).second->flipHorizontal(!flip);
            }
        } else if ((*it).first != _ringKey) {
            if ((*it).second->isFlipHorizontal()!=flip) {
                (*it).second->setFrame((*it).second->getSize()-(*it).second->getFrame()-1);
                (*it).second->flipHorizontal(flip);
            }
        }
    }
}

//helper function to reverse frame order bc flipping horizontally flips the whole texture
int Player::calculateFrame(int frame, std::string key) {
    if ((_horizFlip && key != _ringKey && key != _staffKey) ||
        (key == _staffKey && !_horizFlip))  {
        return _animNodes[key]->getSize()-frame-1;
    } else {
        return frame;
    }
}
