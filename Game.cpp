#include "game.h"

// Charge method - Increasing radius, decreasing ship move speed, increasing dmg.

void LoadError(const char* item) {
	std::cout << item << " not loaded." << std::endl;
}

Game::Game() {
	isLeftPressed = false;
	isDownPressed = false;
	isUpPressed = false;
	isRightPressed = false;
	isGamePaused = false;
	isPlaying = false;

	playerPos = sf::Vector2f(0, 0);
	lastMousePos = sf::Vector2f(0, 0);
	firstBulletPoint = sf::Vector2f(0, 0);
	secondBulletPoint = sf::Vector2f(0, 0);
	firstBulletVector = sf::Vector2f(0, 0);
	secondBulletVector = sf::Vector2f(0, 0);
	world = new sfp::World(sf::Vector2f(0, 0));
}

Game::~Game() {}

void Game::InitializeGame() {
	std::cout << "Initialising game..." << std::endl;
	scoreManager = std::make_unique<ScoreManager>();
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
	playerScoreText.setString(std::to_string(scoreManager->GetPlayerScore()));
	playerScoreText.setCharacterSize(24);
	playerScoreText.setFillColor(sf::Color::Green);
	playerScoreText.setPosition(500, 910);

	opponentScoreText.setFont(font);
	opponentScoreText.setString(std::to_string(scoreManager->GetOpponentScore()));
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
		world->AddPhysicsBody(playerSprite);
	}

	if (!opponentTexture.loadFromFile("assets/Sprites/Characters/enemy-ship.png")) {
		LoadError("Enemy Texture");
	} else {
		opponentSprite.setTexture(opponentTexture);
		opponentSprite.setOrigin(40, 40);
		opponentSprite.setRotation(180);
		opponentSprite.setCenter(sf::Vector2f(440, 180));
		world->AddPhysicsBody(opponentSprite);
	}

	opponentSprite.applyImpulse(sf::Vector2f(0, -10));

	playerBullet.setSize(Vector2f(10, 10));
	playerBullet.setStatic(true);
	world->AddPhysicsBody(playerBullet);

	opponentBullet.setSize(Vector2f(10, 10));
	opponentBullet.setStatic(true);
	world->AddPhysicsBody(opponentBullet);

	return;
}

void Game::Update() {
	sf::Clock gameClock;
	while (window.isOpen()) {

		Time lastTime = gameClock.getElapsedTime();
		bool done = false;
		while (!done) {
			Time current = gameClock.getElapsedTime();
			unsigned int deltaMs = (current - lastTime).asMilliseconds();
			if (deltaMs == 0) {
				continue;
			}
			lastTime = current;
			world->UpdatePhysics(deltaMs);
			window.clear();
			done = true;
		}

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

			if (event.type == sf::Event::MouseButtonReleased) {
				MouseButtonReleaseEventCheck(event.mouseButton);
			}
		}

		UpdatePlayerMovement();
		UpdatePlayerRotation();
		UpdatePlayerScore();
		UpdateOpponentScore();

		window.draw(bgSprite);
		window.draw(playerScoreText);
		window.draw(opponentScoreText);
		window.draw(playerSprite);
		window.draw(opponentSprite);
		window.draw(playerBullet);
		window.draw(opponentBullet);
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

	if (offsetX != 0.0f || offsetY != 0.0f) { playerSprite.move(sf::Vector2f(offsetX, offsetY)); }
	return;
}

void Game::MouseMoveEventCheck(sf::Event::MouseMoveEvent mouse) {
	lastMousePos = sf::Vector2f(mouse.x, mouse.y);
	return;
}

void Game::MouseButtonPressEventCheck(sf::Event::MouseButtonEvent mButton) {
	if (mButton.button == sf::Mouse::Left) {
		firstBulletPoint = sf::Vector2f(mButton.x, mButton.y);
	}

	return;
}

void Game::MouseButtonReleaseEventCheck(sf::Event::MouseButtonEvent mButton) {
	if (mButton.button == sf::Mouse::Left) {
		secondBulletPoint = sf::Vector2f(mButton.x, mButton.y);
		FireBullet();
	}
	return;
}

void Game::FireBullet() {
	std::cout << "GO BULLET\n";
	sf::Vector2f playerPosition = playerSprite.getCenter();
	playerPosition.x -= 40;
	playerPosition.y -= 40;

	sf::Vector2f dirVector = sf::Vector2f(((firstBulletPoint.x - playerPosition.x) / 25), ((firstBulletPoint.y - playerPosition.y) / 25));

	playerBullet.setCenter(playerPosition);
	playerBullet.setStatic(false);
	playerBullet.setVelocity(dirVector);
	return;
}

void Game::UpdatePlayerRotation() {
	playerPos = playerSprite.getCenter();
	playerPos.x -= 40;
	playerPos.y -= 40;
	double angle = std::atan2((lastMousePos.y - playerPos.y), (lastMousePos.x - playerPos.x));
	angle = (angle * 180 / PI) + 90;
	playerSprite.setRotation(angle);
	return;
}

void Game::UpdatePlayerScore() {
	playerScoreText.setString(std::to_string(playerSprite.getCenter().x));
	return;
}

void Game::UpdateOpponentScore() {
	opponentScoreText.setString(std::to_string(playerSprite.getCenter().y));
	return;
}