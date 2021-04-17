#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <cugl/cugl.h>
#include "Element.h"
using namespace cugl;

class Projectile : public cugl::physics2::CapsuleObstacle {
private:

    float _drawscale;

	std::shared_ptr<cugl::scene2::SceneNode> _sceneNode;
	std::shared_ptr<cugl::Texture> _texture;
	int _playerID; //id of the player who activated/shot the projectile, also id of projectile since each player has proj
    Element _preyElement;
    bool _isGone;

public:
	void setTextures(const std::shared_ptr<cugl::Texture>& projectileTexture);

	std::shared_ptr<cugl::scene2::SceneNode> getSceneNode() {
		return _sceneNode;
	}

    int getPlayerID() { return _playerID; };

    Element getPreyElement() { return _preyElement; };

    void setPreyElement(Element element) { _preyElement = element; };

    void setIsGone(bool gone) { _isGone = gone; };
    bool getIsGone() { return _isGone; };

    void update(float delta) override;

#pragma mark Constructors
    /**
     * Creates an projectile with the default values.
     *
     * To properly initialize the projectile, you should call the init
     * method.
     */
    Projectile(void) : CapsuleObstacle(), _drawscale(1.0f) { }

    /**
     * Disposes the projectile, releasing all resources.
     */
    virtual ~Projectile(void) { dispose(); }

    /**
     * Disposes the projectile, releasing all resources.
     */
    void dispose();

    /**
     * Initializes a new projectile
     *     *
     * @return true if the initialization was successful
     */
    virtual bool init(const cugl::Vec2 pos, const cugl::Size size, int playerId);

    /**
     * Returns a newly allocated projectile
     *
     *
     * @return a newly allocated projectile
     */
    static std::shared_ptr<Projectile> alloc(const cugl::Vec2 pos, const cugl::Size size, int playerId) {
        std::shared_ptr<Projectile> result = std::make_shared<Projectile>();
        return (result->init(pos, size, playerId) ? result : nullptr);
    }

#pragma mark -
#pragma mark Physics

    /**
     * Sets the ratio of the sprite to the physics body
     *
     *
     * @param scale The ratio of the  sprite to the physics body
     */
    void setDrawScale(float scale);

    /**
     * Returns the ratio of the sprite to the physics body
     *
     *
     * @return the ratio of the sprite to the physics body
     */
    float getDrawScale() const { return _drawscale; }


};



#endif /* PROJECTILE_H */