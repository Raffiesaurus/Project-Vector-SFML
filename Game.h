#pragma once

#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include "ScoreManager.h"
#include "Spaceship.h"

#define WINDOW_WIDTH 540
#define WINDOW_HEIGHT 960

# define PI 3.14159265358979323846 

class Game {

public:

	Game();
	~Game();

	void InitializeGame();
	void InitializeScreen();
	void InitializePlayers();
	void Update();

	void KeyboardPressEventCheck(sf::Event::KeyEvent key);
	void KeyboardReleaseEventCheck(sf::Event::KeyEvent key);

	void MouseMoveEventCheck(sf::Event::MouseMoveEvent mouse);
	void MouseButtonPressEventCheck(sf::Event::MouseButtonEvent mButton);
	void MouseButtonReleaseEventCheck(sf::Event::MouseButtonEvent mButton);

	void FireBullet();

	void UpdatePlayerMovement();
	void UpdatePlayerRotation();

	void UpdatePlayerScore();
	void UpdateOpponentScore();

private:

	std::unique_ptr<ScoreManager> scoreManager;

	bool isLeftPressed;
	bool isRightPressed;
	bool isUpPressed;
	bool isDownPressed;

	bool isGamePaused;
	bool isPlaying;

	sf::RenderWindow window;

	sf::Font font;

	sf::Text playerScoreText;
	sf::Text opponentScoreText;

	sf::Texture playerTexture;
	sf::Texture opponentTexture;
	sf::Texture bgTexture;

	sfp::PhysicsSprite playerSprite;
	sfp::PhysicsSprite opponentSprite;

	sfp::PhysicsCircle playerBullet;
	sfp::PhysicsCircle opponentBullet;

	sf::Sprite bgSprite;

	sf::Vector2f playerPos;
	sf::Vector2f lastMousePos;
	sf::Vector2f firstBulletPoint;
	sf::Vector2f secondBulletPoint;
	sf::Vector2f firstBulletVector;
	sf::Vector2f secondBulletVector;

	sfp::World* world;
};

