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
	};

	NetworkManager(std::string ip, int port);
	~NetworkManager();

	int Initialize();

	bool WaitForGameStartResponse();

	void SendPositionData(PacketData data);

	PacketData GetPositionData();

private:

	sf::IpAddress serverIp;
	unsigned short serverPort;

	sf::TcpSocket tcpSocket;
	sf::Socket::Status TcpSocketStatus;

	sf::UdpSocket udpSocket;

};

