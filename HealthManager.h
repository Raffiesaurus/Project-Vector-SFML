#pragma once
class HealthManager {
public:

	HealthManager();
	~HealthManager();

	void UpdateHealth(bool player, int increaseVal);

	int GetPlayerHealth();

	int GetOpponentHealth();

private:

	int playerHealth;
	int opponentHealth;
};

