#pragma once

#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>

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

	void UpdatePlayerMovement();
	void UpdatePlayerRotation();

	void UpdatePlayerScore();
	void UpdateOpponentScore();

private:

	int playerScore;
	int opponentScore;

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

	sf::Sprite playerSprite;
	sf::Sprite opponentSprite;
	sf::Sprite bgSprite;

	sf::Vector2f playerPos;
	sf::Vector2f lastMousePos;
};

