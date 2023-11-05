#pragma once
class ScoreManager {
public:

	ScoreManager();
	~ScoreManager();

	void UpdateScore(bool player, int increaseVal);

	int GetPlayerScore();

	int GetOpponentScore();

private:

	int playerScore;
	int opponentScore;
};

