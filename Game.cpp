#include "game.h"

// Charge method - Increasing radius, decreasing ship move speed, increasing dmg.

void LoadError(const char* item) {
	std::cout << item << " not loaded." << std::endl;
}

Game::Game(std::string ip, int port) {
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
	networkManager = std::make_unique<NetworkManager>(ip, port);
	healthManager = std::make_unique<HealthManager>();

	game_state = MENU;
}

Game::~Game() {}

void Game::InitializeGame() {
	sf::Context context;
	std::cout << "Initialising game..." << std::endl;
	SetupMenuScreen();
	SetupLobbyScreen();
	SetupGameScreen();
	InitializePlayers();
	InitializeScreen();
	Update();
}

void Game::SetupMenuScreen() {

	gameName.setFont(font);
	gameName.setString("PROJECT VECTOR");
	gameName.setCharacterSize(60);
	gameName.setFillColor(sf::Color::Blue);
	gameName.setOutlineColor(sf::Color::White);
	gameName.setOutlineThickness(1.5);
	gameName.setPosition(sf::Vector2f(20.0f, 100.0f));

	startButton.setSize(sf::Vector2f(250, 50));
	startButton.setPosition(sf::Vector2f(140.0f, 750.f));
	startButton.setFillColor(sf::Color::Green);

	startText.setFont(font);
	startText.setString("START");
	startText.setCharacterSize(24);
	startText.setFillColor(sf::Color::White);
	startText.setOutlineColor(sf::Color::Black);
	startText.setOutlineThickness(1.5);
	startText.setPosition(sf::Vector2f(227.5f, 758.5f));

	exitButton.setSize(sf::Vector2f(250, 50));
	exitButton.setPosition(sf::Vector2f(140.0f, 850.f));
	exitButton.setFillColor(sf::Color::Red);

	exitText.setFont(font);
	exitText.setString("EXIT");
	exitText.setCharacterSize(24);
	exitText.setFillColor(sf::Color::White);
	exitText.setOutlineColor(sf::Color::Black);
	exitText.setOutlineThickness(1.5);
	exitText.setPosition(sf::Vector2f(240.0f, 858.5f));
}

void Game::SetupLobbyScreen() {
	waitingText.setFont(font);
	waitingText.setString("WAITING FOR PLAYER TO CONNECT");
	waitingText.setCharacterSize(25);
	waitingText.setFillColor(sf::Color::Blue);
	waitingText.setOutlineColor(sf::Color::White);
	waitingText.setOutlineThickness(1.5);
	waitingText.setPosition(sf::Vector2f(65.0f, 100.0f));
}

void Game::InitializeScreen() {
	window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Project Vector");
	window.setFramerateLimit(60);
	window.setActive(false);
	if (!font.loadFromFile("assets/Fonts/Roboto.ttf")) { LoadError("Font"); }
	sf::Thread thread(&Game::RenderingThread, this);
	thread.launch();

	while (window.isOpen()) {
		sf::Event event;

		if (game_state == MENU) {
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) { window.close(); }
				if (event.type == sf::Event::MouseButtonPressed) {
					MouseButtonPressEventCheck(event.mouseButton);
					sf::Vector2f clickPos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
					if (startButton.getGlobalBounds().contains(clickPos)) {
						SwitchToLobby();
						event.type = sf::Event::GainedFocus;
					}
					if (exitButton.getGlobalBounds().contains(clickPos)) {
						window.close();
					}
				}
			}
		}

		else if (game_state == LOBBY) {
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) { window.close(); }
			}
		}

		else if (game_state == IN_GAME) {
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

		}

		else if (game_state == GAME_OVER) {

			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) { window.close(); }
			}

		}
	}
}

void Game::SetupGameScreen() {

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
		playerSprite.setSize(sf::Vector2f(80, 80));
		world->AddPhysicsBody(playerSprite);
	}

	if (!opponentTexture.loadFromFile("assets/Sprites/Characters/enemy-ship.png")) {
		LoadError("Enemy Texture");
	} else {
		opponentSprite.setTexture(opponentTexture);
		opponentSprite.setOrigin(40, 40);
		opponentSprite.setRotation(180);
		opponentSprite.setCenter(sf::Vector2f(440, 180));
		opponentSprite.setSize(sf::Vector2f(80, 80));
		world->AddPhysicsBody(opponentSprite);
	}

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

void Game::SwitchToLobby() {
	game_state = MID_LOBBY;
	bool status = networkManager->Initialize();
	if (status) {
		game_state = LOBBY;
		bool gameStartStaus = networkManager->WaitForGameStartResponse();
		if (gameStartStaus) {
			game_state = IN_GAME;
		} else {
			game_state = MENU;
		}
	} else {
		game_state = MENU;
	}
}

void Game::RenderingThread() {
	window.setActive(true);
	sf::Clock gameClock;
	NetworkManager::PositionData data, oppPosData;
	while (window.isOpen()) {
		if (game_state == MENU) {
			window.draw(startButton);
			window.draw(exitButton);
			window.draw(startText);
			window.draw(exitText);
			window.draw(gameName);
		}

		else if (game_state == LOBBY) {
			window.draw(waitingText);
		}

		else if (game_state == IN_GAME) {
			Time lastTime = gameClock.getElapsedTime();
			bool done = false;
			while (!done) {
				Time current = gameClock.getElapsedTime();
				deltaMs = (current - lastTime).asMilliseconds();
				if (deltaMs == 0) {
					continue;
				}
				lastTime = current;
				world->UpdatePhysics(deltaMs);
				window.clear();
				done = true;
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
			data.x = playerSprite.getCenter().x;
			data.y = playerSprite.getCenter().y;
			networkManager->SendPositionData(data);
			oppPosData = networkManager->GetPositionData();
			if (!isnan(oppPosData.x) && !isnan(oppPosData.y)) {
				if (oppPosData.x != 100000 && oppPosData.y != 100000) {
					UpdateOpponentShipPosition(oppPosData.x, oppPosData.y);
				}
			}
		}

		else if (game_state == GAME_OVER) {

		}

		window.display();
		window.clear();
	}
}

void Game::Update() {

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
		} else if (playerBullet.getCenter().y < 0 || playerBullet.getCenter().y > 1000 || playerBullet.getCenter().x < -50 || playerBullet.getCenter().x > 550) {
			playerBullet.setStatic(true);
			sf::Vector2f playerPosition = playerSprite.getCenter();
			playerPosition.x -= 40;
			playerPosition.y -= 40;
			playerBullet.setCenter(playerPosition);
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
		game_state = GAME_OVER;
	}

	if (healthManager->GetOpponentHealth() <= 0) {
		std::cout << " WINNER WINNER VEGAN DINNER \n";
		isGameOver = true;
		game_state = GAME_OVER;
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
		//FireBullet();
	}
	return;
}

void Game::FireBullet() {
	if (isGameOver || !playerBullet.getStatic()) {
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

void Game::UpdateOpponentShipPosition(float x, float y) {
	opponentSprite.setCenter(sf::Vector2f(x, y));
}

void Game::UpdateOpponentBulletPosition(float x, float y) {
	opponentBullet.setCenter(sf::Vector2f(x, y));
}