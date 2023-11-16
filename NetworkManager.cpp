#include "NetworkManager.h"

NetworkManager::NetworkManager(std::string ip, int port) {
	serverIp = ip;
	serverPort = port;
	TcpSocketStatus = sf::Socket::Status::NotReady;
}

NetworkManager::~NetworkManager() {

}

int NetworkManager::Initialize() {
	std::cout << "Initializing client " << serverIp << " " << serverPort << std::endl;
	udpSocket.setBlocking(false);
	TcpSocketStatus = tcpSocket.connect(serverIp, serverPort);
	if (TcpSocketStatus != sf::Socket::Done) {
		std::cout << "Error " << TcpSocketStatus;
		return -1;
	} else {
		std::cout << "Connected\n";
		char data[2];
		std::size_t received;

		if (udpSocket.bind(tcpSocket.getLocalPort()) != sf::Socket::Done) {
			std::cerr << "Error binding UDP socket to port " << tcpSocket.getLocalPort() << std::endl;
			return 1;
		} else {
			std::cout << "UDP Listening with port " << tcpSocket.getLocalPort() << std::endl;
		}

		if (tcpSocket.receive(data, 2, received) == sf::Socket::Done) {
			std::cout << "Received " << received << " bytes" << " message: " << data << std::endl;
			return std::stoi(data);
		}
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
	if (strcmp(data, "0") == 0) {
		return true;
	} else {
		return false;
	}
}

void NetworkManager::SendPositionData(PacketData playerData) {
	std::cout << "\nData sending: " << playerData.playerNumber << " " << playerData.spritePosX << " " << playerData.spritePosY << " " << playerData.bulletPosX << " " << playerData.bulletPosY;
	sf::Packet packet;
	packet << playerData.playerNumber << playerData.spritePosX << playerData.spritePosY << playerData.bulletPosX << playerData.bulletPosY;
	//if (udpSocket.send(packet, serverIp, serverPort) != sf::Socket::Done) {
	//	//std::cout << "\nError Sending";
	//} else {
	//	std::cout << "\nData Sent";
	//}
	udpSocket.send(packet, serverIp, serverPort);
	return;
}

NetworkManager::PacketData NetworkManager::GetPositionData() {
	sf::Packet packet;

	//if (udpSocket.receive(packet, serverIp, serverPort) != sf::Socket::Done) {
	//	//std::cout << "\nError Recieving";
	//	return data;
	//} else {
	//	std::cout << "\nData Received: ";
	//	packet >> data.x >> data.y;
	//	std::cout << data.x << "  " << data.y;
	//	return data;
	//}
	udpSocket.receive(packet, serverIp, serverPort);
	PacketData playerData;
	packet >> playerData.playerNumber >> playerData.spritePosX >> playerData.spritePosY >> playerData.bulletPosX >> playerData.bulletPosY;
	std::cout << "\nData received from player: " << playerData.playerNumber << " " << playerData.spritePosX << " " << playerData.spritePosY << " " << playerData.bulletPosX << " " << playerData.bulletPosY;
	
	return playerData;
}