#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

constexpr int TCP_MESSAGE_SIZE = 7;

class NetworkManager {

public:
	struct PacketData {
		int playerNumber;
		float spritePosX;
		float spritePosY;
		float bulletPosX;
		float bulletPosY;
		float rotationAngle;
		int mHealth;
		int oHealth;
	};

	enum NetworkEvent {
		PlayerOne = 111,
		PlayerTwo = 222,
		Win = 333,
		Lose = 444,
		PlayOn = 555,
		GameStart = 666,
		Hit = 777,
		Check = 888,
		Error = -111,
	};

	NetworkManager(std::string ip, int port);
	~NetworkManager();

	int Initialize(int hp);

	bool WaitForGameStartResponse();

	void SendData(PacketData data);

	void SendHitEvent();

	PacketData GetData();

	NetworkManager::NetworkEvent CheckDefWinMessage();

	void OnReturnToLobby();

private:

	sf::IpAddress serverIp;
	unsigned short serverPort;

	sf::TcpSocket tcpSocket;
	sf::Socket::Status tcpSocketStatus;

	sf::UdpSocket udpSocket;

};