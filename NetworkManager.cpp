#include "NetworkManager.h"

NetworkManager::NetworkManager(std::string ip, int port) {
	serverIp = ip;
	serverPort = port;
	tcpSocketStatus = sf::Socket::Status::NotReady;
}

NetworkManager::~NetworkManager() {
	tcpSocket.disconnect();
	udpSocket.unbind();
}

//sf::Packet& operator <<(sf::Packet& packet, const NetworkManager::PacketData& data) {
//	return packet << data.playerNumber << data.spritePosX << data.spritePosY << data.bulletPosX << data.bulletPosY << data.rotationAngle << data.health;
//}
//
//sf::Packet& operator >>(sf::Packet& packet, NetworkManager::PacketData& data) {
//	return packet >> data.playerNumber >> data.spritePosX >> data.spritePosY >> data.bulletPosX >> data.bulletPosY >> data.rotationAngle >> data.health;
//}

//std::ostream& operator<<(std::ostream& os, const NetworkManager::PacketData& data) {
//	return os << data.playerNumber << " " << data.bulletPosX << " " << data.bulletPosY << " " << data.health << " " << data.rotationAngle << " " << data.spritePosX << " " << data.spritePosY << std::endl;
//}

int NetworkManager::Initialize(int hp) {
	std::cout << "Initializing client " << serverIp << " " << serverPort << std::endl;
	udpSocket.setBlocking(false);
	tcpSocketStatus = tcpSocket.connect(serverIp, serverPort);
	if (tcpSocketStatus != sf::Socket::Done) {
		std::cout << "Error " << tcpSocketStatus;
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
		std::string hpVal = std::to_string(hp);
		char const* hpData = hpVal.c_str();
		if (tcpSocket.send(hpData, sizeof(float)) != sf::Socket::Done) {
			std::cerr << "Error sending data to server." << std::endl;
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

	if (tcpSocket.receive(data, 2, received) != sf::Socket::Done) {
		// error...
	}
	if (strcmp(data, "0") == 0) {
		return true;
	} else {
		return false;
	}
}

void NetworkManager::SendData(PacketData data) {
	sf::Packet packet;
	packet << data.playerNumber << data.spritePosX << data.spritePosY << data.bulletPosX << data.bulletPosY << data.rotationAngle;
	udpSocket.send(packet, serverIp, serverPort);
	return;
}

void NetworkManager::SendHitEvent() {
	sf::Packet packet;
	const char* hit = "Hit";
	packet << hit;
	udpSocket.send(packet, serverIp, serverPort);
	return;
}

NetworkManager::PacketData NetworkManager::GetData() {
	sf::Packet packet;
	sf::IpAddress ipAddr;
	unsigned short port;
	udpSocket.receive(packet, ipAddr, port);
	PacketData data;
	packet >> data.playerNumber >> data.spritePosX >> data.spritePosY >> data.bulletPosX >> data.bulletPosY >> data.rotationAngle >> data.mHealth >> data.oHealth;
	return data;
}

void NetworkManager::OnReturnToLobby() {
	tcpSocket.disconnect();
	udpSocket.unbind();
	tcpSocketStatus = sf::Socket::Status::NotReady;
}