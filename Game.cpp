#include "game.h"

void LoadError(const char* item) {
	std::cout << item << " not loaded." << std::endl;
}

Game::Game() {
	opponentScore = 0;
	playerScore = 0;

	isLeftPressed = false;
	isDownPressed = false;
	isUpPressed = false;
	isRightPressed = false;
	isGamePaused = false;
	isPlaying = false;

	playerPos = sf::Vector2f(0, 0);
	lastMousePos = sf::Vector2f(0, 0);
}

Game::~Game() {}

void Game::InitializeGame() {
	std::cout << "Initialising game..." << std::endl;
	InitializeScreen();
	InitializePlayers();
	Update();
}

void Game::InitializeScreen() {
	window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Project Vector");
	window.setFramerateLimit(60);

	if (!font.loadFromFile("assets/Fonts/Roboto.ttf")) { LoadError("Font"); }

	if (!bgTexture.loadFromFile("assets/Sprites/Environment/bg.png")) { LoadError("BG Texture"); } else {
		bgSprite.setTexture(bgTexture);
	}

	playerScoreText.setFont(font);
	playerScoreText.setString(std::to_string(playerScore));
	playerScoreText.setCharacterSize(24);
	playerScoreText.setFillColor(sf::Color::Green);
	playerScoreText.setPosition(500, 910);

	opponentScoreText.setFont(font);
	opponentScoreText.setString(std::to_string(opponentScore));
	opponentScoreText.setCharacterSize(24);
	opponentScoreText.setFillColor(sf::Color::Red);
	opponentScoreText.setPosition(20, 20);

	return;
}

void Game::InitializePlayers() {

	if (!playerTexture.loadFromFile("assets/Sprites/Characters/player-ship.png")) {
		LoadError("Player Texture");
	} else {
		playerSprite.setTexture(playerTexture);
		playerSprite.setOrigin(40, 40);
		playerSprite.setCenter(Vector2f(80, 890));
	}

	if (!opponentTexture.loadFromFile("assets/Sprites/Characters/enemy-ship.png")) {
		LoadError("Enemy Texture");
	} else {
		opponentSprite.setTexture(opponentTexture);
		opponentSprite.setOrigin(40, 40);
		opponentSprite.setRotation(180);
		opponentSprite.setCenter(sf::Vector2f(440, 180));
	}

	return;
}

void Game::Update() {
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) { window.close(); }

			if (event.type == sf::Event::KeyPressed) {
				KeyboardPressEventCheck(event.key);
			}

			if (event.type == sf::Event::KeyReleased) {
				KeyboardReleaseEventCheck(event.key);
			}

			if (event.type == sf::Event::MouseMoved) {
				MouseMoveEventCheck(event.mouseMove);
			}

			if (event.type == sf::Event::MouseButtonPressed) {
				MouseButtonPressEventCheck(event.mouseButton);
			}
		}

		UpdatePlayerMovement();
		UpdatePlayerRotation();
		UpdatePlayerScore();
		UpdateOpponentScore();

		window.clear();
		window.draw(bgSprite);
		window.draw(playerScoreText);
		window.draw(opponentScoreText);
		window.draw(playerSprite);
		window.draw(opponentSprite);
		window.display();
	}
}

void Game::KeyboardPressEventCheck(sf::Event::KeyEvent key) {
	if (key.scancode == sf::Keyboard::Scan::Right || key.scancode == sf::Keyboard::Scan::D) {
		isRightPressed = true;
	}

	if (key.scancode == sf::Keyboard::Scan::Left || key.scancode == sf::Keyboard::Scan::A) {
		isLeftPressed = true;
	}

	if (key.scancode == sf::Keyboard::Scan::Up || key.scancode == sf::Keyboard::Scan::W) {
		isUpPressed = true;
	}

	if (key.scancode == sf::Keyboard::Scan::Down || key.scancode == sf::Keyboard::Scan::S) {
		isDownPressed = true;
	}

	if (key.scancode == sf::Keyboard::Scan::Escape) {
		window.close();
	}
	return;
}

void Game::KeyboardReleaseEventCheck(sf::Event::KeyEvent key) {
	if (key.scancode == sf::Keyboard::Scan::Right || key.scancode == sf::Keyboard::Scan::D) {
		isRightPressed = false;
	}

	if (key.scancode == sf::Keyboard::Scan::Left || key.scancode == sf::Keyboard::Scan::A) {
		isLeftPressed = false;
	}

	if (key.scancode == sf::Keyboard::Scan::Up || key.scancode == sf::Keyboard::Scan::W) {
		isUpPressed = false;
	}

	if (key.scancode == sf::Keyboard::Scan::Down || key.scancode == sf::Keyboard::Scan::S) {
		isDownPressed = false;
	}

	return;
}

void Game::UpdatePlayerMovement() {
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	sf::Vector2f playerPos = playerSprite.getCenter();

	if (isLeftPressed && (playerPos.x - 40) > 40) { offsetX = -1.0f; }

	if (isRightPressed && (playerPos.x + 40) < 560) { offsetX = 1.0f; }

	if (isUpPressed && (playerPos.y - 80) > 0) { offsetY = -1.0f; }

	if (isDownPressed && (playerPos.y - 40) < 860) { offsetY = 1.0f; }

	if ((isLeftPressed && isRightPressed) || (!isLeftPressed && !isRightPressed)) { offsetX = 0.0f; }

	if ((isUpPressed && isDownPressed) || (!isUpPressed && !isDownPressed)) { offsetY = 0.0f; }

	if (offsetX != 0.0f || offsetY != 0.0f) { playerSprite.move(offsetX, offsetY); }
}

void Game::MouseMoveEventCheck(sf::Event::MouseMoveEvent mouse) {
	lastMousePos = sf::Vector2f(mouse.x, mouse.y);
	return;
}

void Game::MouseButtonPressEventCheck(sf::Event::MouseButtonEvent mButton) {
	if (mButton.button == sf::Mouse::Left) {

	}

	return;
}

void Game::UpdatePlayerRotation() {
	playerPos = playerSprite.getCenter();
	playerPos.x -= 40;
	playerPos.y -= 40;
	float angle = std::atan2((lastMousePos.y - playerPos.y), (lastMousePos.x - playerPos.x));
	angle = (angle * 180 / PI) + 90;
	playerSprite.setRotation(angle);
}

void Game::UpdatePlayerScore() {
	playerScoreText.setString(std::to_string(playerSprite.getCenter().x));
}

void Game::UpdateOpponentScore() {
	opponentScoreText.setString(std::to_string(playerSprite.getCenter().y));
}