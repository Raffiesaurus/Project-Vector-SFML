#pragma once

#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <thread>
#include "HealthManager.h"
#include "Spaceship.h"
#include "NetworkManager.h"

#define WINDOW_WIDTH 540
#define WINDOW_HEIGHT 960

# define PI 3.14159265358979323846 

class Game {

public:

	Game(std::string ip, int port);
	~Game();

	void InitializeGame();
	void InitializeScreen();
	void InitializePlayers();
	void SetupMenuScreen();
	void SetupLobbyScreen();
	void SetupGameScreen();
	void RenderingThread();
	void Update();

	void KeyboardPressEventCheck(sf::Event::KeyEvent key);
	void KeyboardReleaseEventCheck(sf::Event::KeyEvent key);

	void MouseMoveEventCheck(sf::Event::MouseMoveEvent mouse);
	void MouseButtonPressEventCheck(sf::Event::MouseButtonEvent mButton);
	void MouseButtonReleaseEventCheck(sf::Event::MouseButtonEvent mButton);

	void FireBullet();

	void UpdatePlayerMovement();
	void UpdatePlayerRotation();

	void UpdateHealthTexts();
	void CheckBallCollision();
	void CheckGameOver();

	void SwitchToLobby();

	void UpdateOtherShipPosition(float x, float y);
	void UpdateOtherBulletPosition(float x, float y);
	void UpdateOtherShipRotation(float angle);
	void UpdateOtherShipHealth(float hp);

private:

	enum GAME_PHASE {
		MENU = 0,
		MID_LOBBY,
		LOBBY,
		IN_GAME,
		GAME_OVER,
		CONNECTION_LOST_WIN,
		CONNECTION_LOST_LOSE,
	};

	std::thread gameUpdateThread;

	std::unique_ptr<HealthManager> healthManager;
	std::unique_ptr<NetworkManager> networkManager;

	GAME_PHASE game_state;

	bool isLeftPressed;
	bool isRightPressed;
	bool isUpPressed;
	bool isDownPressed;

	bool isGamePaused;
	bool isPlaying;
	bool isGameOver;

	float playerMoveSpeed;
	float bulletSpeed;
	int bulletDmg;
	int shotCount;
	int playerNumber;

	unsigned int deltaMs;

	sf::RenderWindow window;

	sf::Font font;

	sf::Text gameName;
	sf::Text startText;
	sf::Text exitText;
	sf::Text lobbyText;
	sf::Text waitingText;
	sf::Text playerHealthText;
	sf::Text opponentHealthText;

	sf::Texture playerTexture;
	sf::Texture opponentTexture;
	sf::Texture bgTexture;

	sf::RectangleShape startButton;
	sf::RectangleShape exitButton;

	sfp::PhysicsSprite shipSprites[2];

	sfp::PhysicsCircle bulletCircles[2];

	sf::Sprite bgSprite;

	sf::Vector2f playerPos;
	sf::Vector2f lastMousePos;
	sf::Vector2f firstBulletPoint;
	sf::Vector2f secondBulletPoint;
	sf::Vector2f firstBulletVector;
	sf::Vector2f secondBulletVector;

	sfp::World* world;
};