#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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

	NetworkManager(std::string ip, int port);
	~NetworkManager();

	int Initialize(int hp);

	bool WaitForGameStartResponse();

	void SendData(PacketData data);

	void SendHitEvent();

	PacketData GetData();

private:

	sf::IpAddress serverIp;
	unsigned short serverPort;

	sf::TcpSocket tcpSocket;
	sf::Socket::Status TcpSocketStatus;

	sf::UdpSocket udpSocket;

};