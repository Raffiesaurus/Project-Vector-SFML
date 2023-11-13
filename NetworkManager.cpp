#include "NetworkManager.h"

NetworkManager::NetworkManager(std::string ip, int port) {
	serverIp = ip;
	serverPort = port;
	TcpSocketStatus = sf::Socket::Status::NotReady;
}

NetworkManager::~NetworkManager() {

}

bool NetworkManager::Initialize() {
	std::cout << "Initializing client " << serverIp << " " << serverPort << std::endl;

	TcpSocketStatus = tcpSocket.connect(serverIp, serverPort);

	if (TcpSocketStatus != sf::Socket::Done) {
		// error...
		std::cout << "Error " << TcpSocketStatus;
		return false;
	} else {
		std::cout << "Connected";
		return true;
	}
}

bool NetworkManager::WaitForGameStartResponse() {
	std::cout << "Waiting for response.... " << std::endl;

	char data[2];
	std::size_t received;

	// TCP socket:
	if (tcpSocket.receive(data, 2, received) != sf::Socket::Done) {
		// error...
	}
	std::cout << "Received " << received << " bytes" << " message: " << data << std::endl;
	if (strcmp(data, "1") == 0) {
		return true;
	} else {
		return false;
	}

	
}