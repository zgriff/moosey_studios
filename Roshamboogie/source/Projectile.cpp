#include <stdio.h>
#include <cugl/cugl.h>
#include "Projectile.h"

using namespace cugl;

void Projectile::setTextures(const std::shared_ptr<Texture>& projectileTexture) {
    _sceneNode = scene2::PolygonNode::allocWithTexture(projectileTexture);
    _sceneNode->setAnchor(Vec2::ANCHOR_CENTER);
    _texture = projectileTexture;
    _body->SetUserData(this);

}

bool Projectile::init(const cugl::Vec2 pos, const cugl::Size size, int playerId) {
    if (physics2::CapsuleObstacle::init(pos, size)) {
        setSensor(true);
        setName("projectile");
        _playerID = playerId;
        CULog("player id is %d", _playerID);
        return true;
    }
    return false;
}

void Projectile::dispose() {
    if (_sceneNode != nullptr) {
        _sceneNode->dispose();
        _sceneNode = nullptr;
    }
    _texture = nullptr;
}

void Projectile::setDrawScale(float scale) {
    _drawscale = scale;
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition() * _drawscale);
    }
}

void Projectile::update(float delta) {
    Obstacle::update(delta);
    if (_sceneNode != nullptr) {
        _sceneNode->setPosition(getPosition() * _drawscale);
    }
}