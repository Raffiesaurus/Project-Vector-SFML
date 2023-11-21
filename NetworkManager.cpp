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

sf::Packet& operator <<(sf::Packet& packet, const NetworkManager::PacketData& data) {
	return packet << data.playerNumber << data.spritePosX << data.spritePosY << data.bulletPosX << data.bulletPosY << data.rotationAngle;
}

sf::Packet& operator >>(sf::Packet& packet, NetworkManager::PacketData& data) {
	return packet >> data.playerNumber >> data.spritePosX >> data.spritePosY >> data.bulletPosX >> data.bulletPosY >> data.rotationAngle >> data.mHealth >> data.oHealth;
}

std::ostream& operator<<(std::ostream& os, const NetworkManager::PacketData& data) {
	return os << data.playerNumber << data.spritePosX << data.spritePosY << data.bulletPosX << data.bulletPosY << data.rotationAngle << data.mHealth << data.oHealth;
}

int NetworkManager::Initialize(int hp) {
	std::cout << "Initializing client " << serverIp << " " << serverPort << std::endl;
	tcpSocket.setBlocking(true);
	udpSocket.setBlocking(false);
	tcpSocketStatus = tcpSocket.connect(serverIp, serverPort);
	if (tcpSocketStatus != sf::Socket::Done) {
		std::cout << "Cannot connect to server." << std::endl;
		return NetworkEvent::Error;
	} else {
		std::cout << "Connected\n";
		char data[TCP_MESSAGE_SIZE];
		std::size_t received;

		if (udpSocket.bind(tcpSocket.getLocalPort()) != sf::Socket::Done) {
			std::cerr << "Error binding UDP socket to port " << tcpSocket.getLocalPort() << std::endl;
			return NetworkEvent::Error;
		} else {
			std::cout << "UDP Listening with port " << udpSocket.getLocalPort() << std::endl;
		}
		std::string hpVal = std::to_string(hp);
		char const* hpData = hpVal.c_str();
		if (tcpSocket.send(hpData, sizeof(float)) != sf::Socket::Done) {
			std::cerr << "Error sending data to server." << std::endl;
		}
		if (tcpSocket.receive(data, TCP_MESSAGE_SIZE, received) == sf::Socket::Done) {
			std::cout << "Received " << received << " bytes" << " message: " << data << std::endl;
			if (std::stoi(data) == NetworkEvent::PlayerOne) {
				return 0;
			} else if (std::stoi(data) == NetworkEvent::PlayerTwo) {
				return 1;
			}
		}
	}
}

bool NetworkManager::WaitForGameStartResponse() {
	std::cout << "Waiting for response.... " << std::endl;

	char data[TCP_MESSAGE_SIZE];
	std::size_t received;

	if (tcpSocket.receive(data, TCP_MESSAGE_SIZE, received) == sf::Socket::Done) {
		if (std::stoi(data) == NetworkEvent::GameStart) {
			tcpSocket.setBlocking(false);
			return true;
		} else {
			return false;
		}
	}
}

void NetworkManager::SendData(PacketData data) {
	sf::Packet packet;
	packet << data;
	udpSocket.send(packet, serverIp, serverPort);
	return;
}

void NetworkManager::SendHitEvent() {
	sf::Packet packet;
	NetworkEvent hit = NetworkEvent::Hit;
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
	packet >> data;
	return data;
}

NetworkManager::NetworkEvent NetworkManager::CheckDefWinMessage() {
	char data[TCP_MESSAGE_SIZE];
	std::size_t received;
	if (tcpSocket.receive(data, TCP_MESSAGE_SIZE, received) == sf::Socket::Done) {
		std::cout << "Received " << received << " bytes" << " message: " << data << std::endl;
		switch (std::stoi(data)) {
		case NetworkEvent::Win:
			return NetworkEvent::Win;
		case NetworkEvent::Lose:
			return NetworkEvent::Lose;
		default:
			return NetworkEvent::PlayOn;
		}
	}
	return NetworkEvent::PlayOn;
}

void NetworkManager::OnReturnToLobby() {
	tcpSocket.disconnect();
	udpSocket.unbind();
	tcpSocketStatus = sf::Socket::Status::NotReady;
}