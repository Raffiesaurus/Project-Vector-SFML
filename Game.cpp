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
	isGameOver = false;

	playerMoveSpeed = 7.5f;
	bulletSpeed = 25.0f;
	bulletDmg = 20;

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
	healthManager = std::make_unique<HealthManager>();
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

	playerHealthText.setFont(font);
	playerHealthText.setString(std::to_string(healthManager->GetPlayerHealth()));
	playerHealthText.setCharacterSize(24);
	playerHealthText.setFillColor(sf::Color::Green);
	playerHealthText.setPosition(500, 910);

	opponentHealthText.setFont(font);
	opponentHealthText.setString(std::to_string(healthManager->GetOpponentHealth()));
	opponentHealthText.setCharacterSize(24);
	opponentHealthText.setFillColor(sf::Color::Red);
	opponentHealthText.setPosition(20, 20);

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

	//opponentSprite.applyImpulse(sf::Vector2f(0, -10));

	playerBullet.setStatic(true);
	playerBullet.setSize(Vector2f(10, 10));
	playerBullet.setCenter(sf::Vector2f(1000, 1000));
	world->AddPhysicsBody(playerBullet);

	opponentBullet.setStatic(true);
	opponentBullet.setSize(Vector2f(10, 10));
	opponentBullet.setCenter(sf::Vector2f(1000, 1000));
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
		if (!isGameOver) {
			CheckBallCollision();
			UpdateHealthTexts();
			CheckGameOver();
			UpdatePlayerMovement();
			UpdatePlayerRotation();
		}

		window.draw(bgSprite);
		window.draw(playerHealthText);
		window.draw(opponentHealthText);
		if (!playerBullet.getStatic())
			window.draw(playerBullet);
		if (!opponentBullet.getStatic())
			window.draw(opponentBullet);
		window.draw(playerSprite);
		window.draw(opponentSprite);
		window.display();
	}
}

void Game::CheckBallCollision() {
	if (!playerBullet.getStatic()) {
		if (playerBullet.collideWith(opponentSprite).hasCollided) {
			playerBullet.setStatic(true);
			sf::Vector2f playerPosition = playerSprite.getCenter();
			playerPosition.x -= 40;
			playerPosition.y -= 40;
			playerBullet.setCenter(playerPosition);
			healthManager->UpdateHealth(false, bulletDmg);
		}
	}

	if (!opponentBullet.getStatic()) {
		if (opponentBullet.collideWith(opponentSprite).hasCollided) {
			opponentBullet.setStatic(true);
			sf::Vector2f opponenetPosition = opponentSprite.getCenter();
			opponenetPosition.x -= 40;
			opponenetPosition.y -= 40;
			opponentBullet.setCenter(opponenetPosition);
			healthManager->UpdateHealth(true, bulletDmg);
		}
	}

	return;
}

void Game::CheckGameOver() {
	if (healthManager->GetPlayerHealth() <= 0) {
		std::cout << " CAN'T BELIEVE YOU'VE DONE THIS \n";
		isGameOver = true;
	}

	if (healthManager->GetOpponentHealth() <= 0) {
		std::cout << " WINNER WINNER VEGAN DINNER \n";
		isGameOver = true;
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

	if (isLeftPressed && (playerPos.x - 40) > 40) { offsetX = -playerMoveSpeed; }

	if (isRightPressed && (playerPos.x + 40) < 560) { offsetX = playerMoveSpeed; }

	if (isUpPressed && (playerPos.y - 80) > 0) { offsetY = -playerMoveSpeed; }

	if (isDownPressed && (playerPos.y - 40) < 860) { offsetY = playerMoveSpeed; }

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
	if (isGameOver) {
		return;
	}
	sf::Vector2f playerPosition = playerSprite.getCenter();
	playerPosition.x -= 40;
	playerPosition.y -= 40;

	sf::Vector2f dirVector = sf::Vector2f((firstBulletPoint.x - playerPosition.x),
		(firstBulletPoint.y - playerPosition.y));
	double len = sqrt(pow(dirVector.x, 2) + pow(dirVector.y, 2));
	dirVector.x *= bulletSpeed / len;
	dirVector.y *= bulletSpeed / len;


	sf::Vector2f startPoint = sf::Vector2f((playerPosition.x + dirVector.x * 0.1), (playerPosition.y + dirVector.y * 0.1));

	playerBullet.setCenter(startPoint);
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

void Game::UpdateHealthTexts() {
	playerHealthText.setString(std::to_string(healthManager->GetPlayerHealth()));
	opponentHealthText.setString(std::to_string(healthManager->GetOpponentHealth()));
	return;
}