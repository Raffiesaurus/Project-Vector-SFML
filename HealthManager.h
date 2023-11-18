#pragma once
class HealthManager {
public:

	HealthManager();
	~HealthManager();

	void UpdateHealth(bool player, int increaseVal);

	void SetHealth(bool player, float hp);

	int GetPlayerHealth();

	int GetOpponentHealth();

private:

	int playerHealth;
	int opponentHealth;
};

