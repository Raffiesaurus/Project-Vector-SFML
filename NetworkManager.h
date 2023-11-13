#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
class NetworkManager {

public:

	NetworkManager(std::string ip, int port);
	~NetworkManager();

	bool Initialize();

	bool WaitForGameStartResponse();

private:

	std::string serverIp;
	int serverPort;

	sf::TcpSocket tcpSocket;
	sf::Socket::Status TcpSocketStatus;

	sf::UdpSocket udpSocket;

};

