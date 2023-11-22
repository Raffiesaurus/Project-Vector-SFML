#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>

constexpr int TCP_MESSAGE_SIZE = 7;

class NetworkManager {

public:
	struct PacketData {
		int playerNumber = 0;
		int mHealth = 0;
		int oHealth = 0;
		float spritePosX = 0;
		float spritePosY = 0;
		float bulletPosX = 0;
		float bulletPosY = 0;
		float rotationAngle = 0;
		float gameTime = 0;
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

	NetworkManager::PacketData RunPrediction(float gameTime);

private:

	sf::IpAddress serverIp;
	unsigned short serverPort;

	sf::TcpSocket tcpSocket;
	sf::Socket::Status tcpSocketStatus;

	sf::UdpSocket udpSocket;

	float lastMessageTime;

	std::vector<PacketData> prevPacketsRecv;
};