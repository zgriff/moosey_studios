
#include <cugl/cugl.h>
#include "AbilityController.h"
#include <Player.h>
#include <Element.h>
#include <stdlib.h>
#include <chrono>
#include <ctime> 

using namespace cugl;

AbilityController::AbilityController() {

}


void AbilityController::updateAbility(std::shared_ptr<cugl::scene2::Label> abilityname) {
	srand(time(NULL));
	int abilityChance = rand() % 100;
	if (abilityChance < 50) {
		_queuedAbility = Ability::AetherAbility;
		abilityname->setText("Aether");
	}
	else {
		_queuedAbility = Ability::SpeedBoost;
		abilityname->setText("Boost");
	}
}

void AbilityController::activateAbility(std::shared_ptr<Player> player) {
	if (_queuedAbility != Ability::NoAbility && player->getCurrElement() != Element::None) {
		player->setOrbScore(0);
		_timer = std::chrono::system_clock::now();
		
		switch (_queuedAbility) {
			case Ability::AetherAbility:
				player->setElement(Element::Aether);
				_activeAbility = Ability::AetherAbility;
				_queuedAbility = Ability::NoAbility;
				break;
			case Ability::SpeedBoost:
				player->setLinearVelocity(player->getLinearVelocity() * 2);
				_activeAbility = Ability::SpeedBoost;
				_queuedAbility = Ability::NoAbility;
				break;
		}
	}
}

void AbilityController::deactivateAbility(std::shared_ptr<Player> player, std::shared_ptr<cugl::scene2::Label> abilityname) {
	if (_activeAbility != Ability::NoAbility) {
		auto curr = std::chrono::system_clock::now();
		std::chrono::duration<double> timePassed = curr - _timer;
		switch (_activeAbility) {
			case Ability::AetherAbility:
				if (timePassed.count() >= 3.0) {
					if (player->getCurrElement() != Element::None) {
						player->setElement(player->getPrevElement());
					}
					//time of ability is up
					_activeAbility = Ability::NoAbility;
					abilityname->setVisible(false);
				}
			break;
			case Ability::SpeedBoost:
				if (timePassed.count() >= 0.25) {
					player->setLinearVelocity(player->getLinearVelocity() * 0.5);
					_activeAbility = Ability::NoAbility;
					abilityname->setVisible(false);
				}
		}
	}
}


void AbilityController::dispose() {

}