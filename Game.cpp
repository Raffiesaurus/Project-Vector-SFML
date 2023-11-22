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
	isWinner = false;

	playerMoveSpeed = 7.5f;
	bulletSpeed = 25.0f;
	bulletDmg = 20;
	playerNumber = 1;
	shotCount = 0;

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
	srand(time(0));
	sf::Context context;
	std::cout << "Initialising game..." << std::endl;
	SetupMenuScreen();
	SetupLobbyScreen();
	SetupGameScreen();
	SetupGameOverScreen();
	InitializePlayers();
	InitializeScreen();
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

void Game::SetupGameOverScreen() {
	menuText.setFont(font);
	menuText.setString("MENU");
	menuText.setCharacterSize(24);
	menuText.setFillColor(sf::Color::White);
	menuText.setOutlineColor(sf::Color::Black);
	menuText.setOutlineThickness(1.5);
	menuText.setPosition(sf::Vector2f(227.5f, 758.5f));

	menuButton.setSize(sf::Vector2f(250, 50));
	menuButton.setPosition(sf::Vector2f(140.0f, 750.f));
	menuButton.setFillColor(sf::Color::Green);

	gameResultText.setFont(font);
	gameResultText.setString("N/A");
	gameResultText.setCharacterSize(42);
	gameResultText.setFillColor(sf::Color::White);
	gameResultText.setOutlineColor(sf::Color::Red);
	gameResultText.setOutlineThickness(1.5);
	gameResultText.setPosition(sf::Vector2f(180.0f, 400.0f));

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
					sf::Vector2f clickPos = sf::Vector2f(event.mouseButton.spritePosX, event.mouseButton.spritePosY);
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
				if (event.type == sf::Event::MouseButtonPressed) {
					sf::Vector2f clickPos = sf::Vector2f(event.mouseButton.spritePosX, event.mouseButton.spritePosY);
					if (menuButton.getGlobalBounds().contains(clickPos)) {
						SwitchToHome();
						event.type = sf::Event::GainedFocus;
					}
					if (exitButton.getGlobalBounds().contains(clickPos)) {
						window.close();
					}
				}
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
		shipSprites[0].setTexture(playerTexture);
		shipSprites[0].setOrigin(40, 40);
		shipSprites[0].setCenter(Vector2f(80, 890));
		shipSprites[0].setSize(sf::Vector2f(80, 80));
		world->AddPhysicsBody(shipSprites[0]);
	}

	if (!opponentTexture.loadFromFile("assets/Sprites/Characters/enemy-ship.png")) {
		LoadError("Enemy Texture");
	} else {
		shipSprites[1].setTexture(opponentTexture);
		shipSprites[1].setOrigin(40, 40);
		shipSprites[1].setCenter(sf::Vector2f(440, 180));
		shipSprites[1].setSize(sf::Vector2f(80, 80));
		world->AddPhysicsBody(shipSprites[1]);
	}

	bulletCircles[0].setSize(Vector2f(10, 10));
	bulletCircles[0].setCenter(sf::Vector2f(10000, 10000));
	world->AddPhysicsBody(bulletCircles[0]);

	bulletCircles[1].setSize(Vector2f(10, 10));
	bulletCircles[1].setCenter(sf::Vector2f(10000, 10000));
	world->AddPhysicsBody(bulletCircles[1]);

	return;
}

void Game::SwitchToLobby() {
	game_state = MID_LOBBY;
	int playerNum = networkManager->Initialize(healthManager->GetPlayerHealth());
	if (playerNum != NetworkManager::NetworkEvent::Error) {
		playerNumber = playerNum;
		std::cout << "I am player : " << playerNumber + 1 << std::endl;
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

void Game::ResetGame() {
	isLeftPressed = false;
	isDownPressed = false;
	isUpPressed = false;
	isRightPressed = false;
	isGamePaused = false;
	isPlaying = false;
	isGameOver = false;
	isWinner = false;

	playerNumber = 1;
	shotCount = 0;
	playerPos = sf::Vector2f(0, 0);
	lastMousePos = sf::Vector2f(0, 0);
	firstBulletPoint = sf::Vector2f(0, 0);
	secondBulletPoint = sf::Vector2f(0, 0);
	firstBulletVector = sf::Vector2f(0, 0);
	secondBulletVector = sf::Vector2f(0, 0);
	//world = new sfp::World(sf::Vector2f(0, 0));
}

void Game::SwitchToHome() {
	networkManager->OnReturnToLobby();
	game_state = MENU;
	ResetGame();
}

void Game::RenderingThread() {
	window.setActive(true);
	Time lastMessageSentTime, lastMessageReceivedTime = sf::seconds(0);
	NetworkManager::PacketData data, serverData;
	sf::Clock gameClock;
	auto lastCheckTime = std::chrono::system_clock::now();
	while (window.isOpen()) {
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - lastCheckTime;
		lastCheckTime = now;
		//std::cout << " " << elapsed_seconds.count() << std::endl;
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
			if (!isGameOver && gameClock.getElapsedTime() > sf::seconds(1)) {
				CheckBallCollision();
				UpdateHealthTexts();
				UpdatePlayerMovement();
				UpdatePlayerRotation();
				auto winCheck = networkManager->CheckDefWinMessage();
				if (winCheck == NetworkManager::NetworkEvent::Win) {
					std::cout << " WINNER WINNER VEGAN DINNER \n";
					isGameOver = true;
					isWinner = true;
					game_state = GAME_OVER;
					gameResultText.setString("YOU WON");
				} else if (winCheck == NetworkManager::NetworkEvent::Lose) {
					std::cout << " CAN'T BELIEVE YOU'VE LOST THIS \n";
					isGameOver = true;
					isWinner = false;
					game_state = GAME_OVER;
					gameResultText.setString("YOU LOST");
				}
			}
			window.draw(bgSprite);
			window.draw(playerHealthText);
			window.draw(opponentHealthText);
			window.draw(bulletCircles[0]);
			window.draw(bulletCircles[1]);
			window.draw(shipSprites[0]);
			window.draw(shipSprites[1]);
			if (gameClock.getElapsedTime() - lastMessageSentTime > sf::seconds(0.5)) {
				lastMessageSentTime = gameClock.getElapsedTime();

				data.playerNumber = playerNumber;
				data.bulletPosX = bulletCircles[playerNumber].getCenter().spritePosX;
				data.bulletPosY = bulletCircles[playerNumber].getCenter().spritePosY;
				data.spritePosX = shipSprites[playerNumber].getCenter().spritePosX;
				data.spritePosY = shipSprites[playerNumber].getCenter().spritePosY;
				data.rotationAngle = shipSprites[playerNumber].getRotation();
				networkManager->SendData(data);

				serverData = networkManager->GetData();
				if (serverData.playerNumber != playerNumber) {
					UpdateOtherShipPosition(serverData.spritePosX, serverData.spritePosY);
					UpdateOtherBulletPosition(serverData.bulletPosX, serverData.bulletPosY);
					UpdateOtherShipRotation(serverData.rotationAngle);
					healthManager->SetHealth(true, serverData.mHealth);
					healthManager->SetHealth(false, serverData.oHealth);
				}
			}
		}

		else if (game_state == GAME_OVER) {
			window.draw(menuButton);
			window.draw(exitButton);
			window.draw(menuText);
			window.draw(exitText);
			window.draw(gameResultText);
		}

		window.display();
		window.clear();
	}
}

void Game::CheckBallCollision() {
	if (bulletCircles[playerNumber].collideWith(shipSprites[!playerNumber]).hasCollided) {
		bulletCircles[playerNumber].setCenter(sf::Vector2f(10000, 10000));
		networkManager->SendHitEvent();
	} else if (bulletCircles[playerNumber].getCenter().spritePosY < 0 || bulletCircles[playerNumber].getCenter().spritePosY > 1000 || bulletCircles[playerNumber].getCenter().spritePosX < -50 || bulletCircles[playerNumber].getCenter().spritePosX > 550) {
		bulletCircles[playerNumber].setCenter(sf::Vector2f(10000, 10000));
	}

	return;
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
	sf::Vector2f playerPos = shipSprites[playerNumber].getCenter();

	if (isLeftPressed && (playerPos.spritePosX - 40) > 40) { offsetX = -playerMoveSpeed; }

	if (isRightPressed && (playerPos.spritePosX + 40) < 560) { offsetX = playerMoveSpeed; }

	if (isUpPressed && (playerPos.spritePosY - 80) > 0) { offsetY = -playerMoveSpeed; }

	if (isDownPressed && (playerPos.spritePosY - 40) < 860) { offsetY = playerMoveSpeed; }

	if ((isLeftPressed && isRightPressed) || (!isLeftPressed && !isRightPressed)) { offsetX = 0.0f; }

	if ((isUpPressed && isDownPressed) || (!isUpPressed && !isDownPressed)) { offsetY = 0.0f; }

	if (offsetX != 0.0f || offsetY != 0.0f) { shipSprites[playerNumber].move(sf::Vector2f(offsetX, offsetY)); }
	return;
}

void Game::MouseMoveEventCheck(sf::Event::MouseMoveEvent mouse) {
	lastMousePos = sf::Vector2f(mouse.spritePosX, mouse.spritePosY);
	return;
}

void Game::MouseButtonPressEventCheck(sf::Event::MouseButtonEvent mButton) {
	if (mButton.button == sf::Mouse::Left) {
		firstBulletPoint = sf::Vector2f(mButton.spritePosX, mButton.spritePosY);
	}

	return;
}

void Game::MouseButtonReleaseEventCheck(sf::Event::MouseButtonEvent mButton) {
	if (mButton.button == sf::Mouse::Left) {
		secondBulletPoint = sf::Vector2f(mButton.spritePosX, mButton.spritePosY);
		FireBullet();
	}
	return;
}

void Game::FireBullet() {
	shotCount++;
	if (isGameOver || shotCount <= 1) {
		return;
	}
	sf::Vector2f playerPosition = shipSprites[playerNumber].getCenter();
	playerPosition.spritePosX -= 40;
	playerPosition.spritePosY -= 40;

	sf::Vector2f dirVector = sf::Vector2f((firstBulletPoint.spritePosX - playerPosition.spritePosX),
		(firstBulletPoint.spritePosY - playerPosition.spritePosY));
	double len = sqrt(pow(dirVector.spritePosX, 2) + pow(dirVector.spritePosY, 2));
	dirVector.spritePosX *= bulletSpeed / len;
	dirVector.spritePosY *= bulletSpeed / len;

	sf::Vector2f startPoint = sf::Vector2f((playerPosition.spritePosX + dirVector.spritePosX * 0.3), (playerPosition.spritePosY + dirVector.spritePosY * 0.3));

	bulletCircles[playerNumber].setCenter(startPoint);
	bulletCircles[playerNumber].setVelocity(dirVector);
	return;
}

void Game::UpdatePlayerRotation() {
	playerPos = shipSprites[playerNumber].getCenter();
	playerPos.spritePosX -= 40;
	playerPos.spritePosY -= 40;
	double angle = std::atan2((lastMousePos.spritePosY - playerPos.spritePosY), (lastMousePos.spritePosX - playerPos.spritePosX));
	angle = (angle * 180 / PI) + 90;
	shipSprites[playerNumber].setRotation(angle);
	return;
}

void Game::UpdateHealthTexts() {
	playerHealthText.setString(std::to_string(healthManager->GetPlayerHealth()));
	opponentHealthText.setString(std::to_string(healthManager->GetOpponentHealth()));
	return;
}

void Game::UpdateOtherShipPosition(float spritePosX, float spritePosY) {
	shipSprites[!playerNumber].setCenter(sf::Vector2f(spritePosX, spritePosY));
}

void Game::UpdateOtherBulletPosition(float spritePosX, float spritePosY) {
	bulletCircles[!playerNumber].setCenter(sf::Vector2f(spritePosX, spritePosY));
}

void Game::UpdateOtherShipRotation(float angle) {
	shipSprites[!playerNumber].setRotation(angle);
}

void Game::UpdateOtherShipHealth(float hp) {
	if (hp >= 0)
		healthManager->SetHealth(false, hp);
}