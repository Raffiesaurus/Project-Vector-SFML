#include "ScoreManager.h"

ScoreManager::ScoreManager() {
	playerScore = 0;
	opponentScore = 0;
}

ScoreManager::~ScoreManager() {}

void ScoreManager::UpdateScore(bool player, int increaseVal) {
	player ? playerScore += increaseVal : opponentScore += increaseVal;
}

int ScoreManager::GetPlayerScore() {
	return playerScore;
}

int ScoreManager::GetOpponentScore() {
	return opponentScore;
}