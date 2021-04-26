
#include <cugl/cugl.h>
#include "AbilityController.h"
#include "Player.h"
#include "Element.h"
#include "Projectile.h"
#include "NetworkController.h"
#include <stdlib.h>
#include <chrono>
#include <ctime> 

using namespace cugl;

AbilityController::AbilityController() {

}


void AbilityController::updateAbility(std::shared_ptr<cugl::scene2::Label> abilityname) {
	srand(time(NULL));
	int abilityChance = rand() % 100;
	if (abilityChance < 0) {
		_queuedAbility = Ability::AetherAbility;
		abilityname->setText("Aether");
	}
	else if (abilityChance < 100) {
		_queuedAbility = Ability::SpeedBoost;
		abilityname->setText("Boost");
	}
	else {
		_queuedAbility = Ability::Projectile;
		abilityname->setText("Projectile");
	}
}

void AbilityController::activateAbility(std::shared_ptr<Player> player) {
	if (_queuedAbility != Ability::NoAbility && player->getCurrElement() != Element::None && !player->getIsTagged()) {
		player->setOrbScore(0);
		_timer = std::chrono::system_clock::now();
		
		switch (_queuedAbility) {
			case Ability::AetherAbility:
				player->setElement(Element::Aether);
				NetworkController::sendElementChange(player->getID(), Element::Aether);
				_activeAbility = Ability::AetherAbility;
				_queuedAbility = Ability::NoAbility;
				break;
			case Ability::SpeedBoost:
			{
				player->setLinearVelocity(player->getLinearVelocity() * 2.0);
				_activeAbility = Ability::SpeedBoost;
				_queuedAbility = Ability::NoAbility;
				break;
			}
			case Ability::Projectile:
            {
				auto projectile = player->getProjectile();
				projectile->setActive(true);
				//projectile->setElement(player->getCurrElement());
				//Aether projectile for now, so prey is everything
				projectile->setPreyElement(Element::None);
				projectile->setPosition(player->getPosition());
				projectile->getSceneNode()->setVisible(true);
				projectile->setLinearVelocity(Vec2::forAngle(player->getAngle() + M_PI/2) * 25);
				projectile->setAngle(player->getAngle());
				projectile->getSceneNode()->setAngle(player->getAngle() + M_PI);
				projectile->setIsGone(false);
				NetworkController::sendProjectileFired(projectile->getPlayerID(), player->getPosition(), player->getAngle(), Element::None);


				CULog("player speed is %f, %f", player->getLinearVelocity().x, player->getLinearVelocity().y);

				/*CULog("player angle is %f", player->getAngle());
				CULog("projectile angle is %f", projectile->getAngle());

				CULog("player position is %f , %f", player->getPosition().x, player->getPosition().y);
				CULog("projectile position is %f , %f", projectile->getPosition().x, projectile->getPosition().y);*/
				_activeAbility = Ability::Projectile;
				_queuedAbility = Ability::NoAbility;
				break;
            }
		}
	}
}

void AbilityController::deactivateAbility(std::shared_ptr<Player> player, std::shared_ptr<cugl::scene2::Label> abilityname) {
	if (_activeAbility != Ability::NoAbility) {
		auto curr = std::chrono::system_clock::now();
		std::chrono::duration<double> timePassed = curr - _timer;
		bool abilityOver = false;
		switch (_activeAbility) {
			//time of ability is up
			case Ability::AetherAbility:
				if (timePassed.count() >= 3.0) {
					if (player->getCurrElement() != Element::None) {
						player->setElement(player->getPrevElement());
						NetworkController::sendElementChange(player->getID(), player->getPrevElement());
					}
					// might want to replace below two lines with abilityOver = true depending on functionality
					_activeAbility = Ability::NoAbility;
					abilityname->setVisible(false);
				}
				break;
			case Ability::SpeedBoost:
				if (timePassed.count() >= 0.25) {
					//player->setLinearVelocity(player->getLinearVelocity() * 0.5);
					_activeAbility = Ability::NoAbility;
					abilityname->setVisible(false);
				}
				break;
			case Ability::Projectile:
				if (timePassed.count() >= 1 || player->getProjectile()->getIsGone()) {
					auto projectile = player->getProjectile();
					projectile->setActive(false);
					projectile->getSceneNode()->setVisible(false);
					projectile->setLinearVelocity(Vec2(0, 0));
					projectile->setPosition(Vec2(0, 0));
					NetworkController::sendProjectileGone(projectile->getPlayerID());
					_activeAbility = Ability::NoAbility;
					abilityname->setVisible(false);
				}
		}
		if (abilityOver) {
			_activeAbility = Ability::NoAbility;
			abilityname->setVisible(false);
		}
	}
}


void AbilityController::dispose() {
}
