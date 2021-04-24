#ifndef __ABILITY_CONTROLLER_H__
#define __ABILITY_CONTROLLER_H__

#include <cugl/cugl.h>
#include "Player.h"
#include "Element.h"
#include "Projectile.h"
#include <time.h>
#include <chrono>
#include <ctime> 

class AbilityController {

public:
	enum Ability {
		AetherAbility, SpeedBoost, Projectile, NoAbility
	};

private:
	std::chrono::system_clock::time_point _timer;
	Ability _activeAbility = NoAbility;
	Ability _queuedAbility = NoAbility;

public:
	AbilityController();

	~AbilityController() { dispose(); };

	void dispose();

	//bool init();

	void updateAbility(std::shared_ptr<cugl::scene2::Label> abilityname);

	Ability getQueuedAbility() { return _queuedAbility; };

	Ability getActiveAbility() { return _activeAbility; };

	void activateAbility(std::shared_ptr<Player> player);

	void deactivateAbility(std::shared_ptr<Player> player, std::shared_ptr<cugl::scene2::Label> abilityname);

};



#endif /* AbilityController_hpp */
