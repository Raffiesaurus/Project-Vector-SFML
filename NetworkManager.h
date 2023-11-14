#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
class NetworkManager {

public:
	struct PositionData {
		float x;
		float y;
	};

	NetworkManager(std::string ip, int port);
	~NetworkManager();

	bool Initialize();

	bool WaitForGameStartResponse();

	void SendPositionData(PositionData data);

	PositionData GetPositionData();

private:

	sf::IpAddress serverIp;
	unsigned short serverPort;

	sf::TcpSocket tcpSocket;
	sf::Socket::Status TcpSocketStatus;

	sf::UdpSocket udpSocket;

};

