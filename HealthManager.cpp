#include "HealthManager.h"

HealthManager::HealthManager() {
	playerHealth = 100;
	opponentHealth = 100;
}

HealthManager::~HealthManager() {}

void HealthManager::UpdateHealth(bool player, int dmg) {
	player ? playerHealth -= dmg : opponentHealth -= dmg;
}

int HealthManager::GetPlayerHealth() {
	return playerHealth;
}

int HealthManager::GetOpponentHealth() {
	return opponentHealth;
}