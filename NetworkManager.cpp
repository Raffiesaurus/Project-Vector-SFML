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
	return packet << data.playerNumber << data.spritePosX << data.spritePosY << data.bulletPosX << data.bulletPosY << data.rotationAngle << data.gameTime;
}

sf::Packet& operator >>(sf::Packet& packet, NetworkManager::PacketData& data) {
	return packet >> data.playerNumber >> data.spritePosX >> data.spritePosY >> data.bulletPosX >> data.bulletPosY >> data.rotationAngle >> data.mHealth >> data.oHealth >> data.gameTime;
}

std::ostream& operator<<(std::ostream& os, const NetworkManager::PacketData& data) {
	return os << data.playerNumber << " " << data.spritePosX << " " << data.spritePosY << " " << data.bulletPosX << " " << data.bulletPosY << " " << data.rotationAngle << " " << data.mHealth << " " << data.oHealth << " " << data.gameTime;
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

void NetworkManager::GetData() {
	sf::Packet packet;
	sf::IpAddress ipAddr;
	unsigned short port;
	udpSocket.receive(packet, ipAddr, port);
	PacketData data;
	packet >> data;
	if (data.gameTime > 0) {
		if (prevPacketsRecv.size() < 2) {
			prevPacketsRecv.push_back(data);
		} else {
			prevPacketsRecv.erase(prevPacketsRecv.begin());
			prevPacketsRecv.push_back(data);
		}
	}
}

NetworkManager::NetworkEvent NetworkManager::CheckDefWinMessage() {
	char data[TCP_MESSAGE_SIZE];
	std::size_t received;
	if (tcpSocket.receive(data, TCP_MESSAGE_SIZE, received) == sf::Socket::Done) {
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

NetworkManager::PacketData NetworkManager::RunPrediction(float gameTime) {

	PacketData invalidData;

	if (prevPacketsRecv.size() <= 0) {
		lastPredictionTime = gameTime;
		return invalidData;
	}

	if (prevPacketsRecv.size() < 2) {
		lastPredictionTime = gameTime;
		return prevPacketsRecv[prevPacketsRecv.size() - 1];
	}

	PacketData* msgOld = &prevPacketsRecv[prevPacketsRecv.size() - 2];
	PacketData* msgNew = &prevPacketsRecv[prevPacketsRecv.size() - 1];

	float time = (msgNew->gameTime - msgOld->gameTime);
	float gameTimeDiff = abs(lastPredictionTime - gameTime);

	// Ship Pos
	float distX = (msgNew->spritePosX - msgOld->spritePosX);
	float distY = (msgNew->spritePosY - msgOld->spritePosY);

	float speedX = distX / time;
	float speedY = distY / time;

	float displacementX = speedX * gameTimeDiff;
	float displacementY = speedY * gameTimeDiff;

	float nextX = msgOld->spritePosX + displacementX;
	float nextY = msgOld->spritePosY + displacementY;

	// Bullet
	//if (msgOld->bulletPosX != 10000 && msgOld->bulletPosY != 10000) {
		float distXB = (msgNew->bulletPosX - msgOld->bulletPosX);
		float distYB = (msgNew->bulletPosY - msgOld->bulletPosY);

		float speedXB = distXB / time;
		float speedYB = distYB / time;

		float displacementXB = speedXB * gameTimeDiff;
		float displacementYB = speedYB * gameTimeDiff;

		float nextXB = msgOld->bulletPosX + displacementXB;
		float nextYB = msgOld->bulletPosY + displacementYB;
	//} else {
		//msgOld->bulletPosX = msgNew->bulletPosX;
		//msgOld->bulletPosY = msgNew->bulletPosY;
	//}

	// Rotation
	float rot = (msgNew->rotationAngle - msgOld->rotationAngle);
	float speedR = rot / time;
	float displacementR = speedR * gameTimeDiff;
	float nextR = msgOld->rotationAngle + displacementR;

	// Check
	/*if ((msgOld->spritePosX != msgNew->spritePosX) || (msgOld->spritePosY != msgNew->spritePosY)) {
		std::cout << "---------------------------------------------------------------------------\n";
		std::cout << gameTime << std::endl;
		std::cout << "Sprite X Old, New : " << msgOld->spritePosX << " " << msgNew->spritePosX << std::endl;
		std::cout << "Sprite Y : " << msgOld->spritePosY << " " << msgNew->spritePosY << std::endl;
		std::cout << "Game Time Old, New : " << msgOld->gameTime << " " << msgNew->gameTime << std::endl;
		std::cout << "Speed X, Y : " << speedX << " " << speedY << std::endl;
		std::cout << "Displacement X, Y : " << displacementX << " " << displacementY << std::endl;
		std::cout << "Next X, Y : " << nextX << " " << nextY << std::endl;
		std::cout << "---------------------------------------------------------------------------\n";
	}*/

	//msgOld->gameTime = gameTime;
	msgOld->spritePosX = nextX;
	msgOld->spritePosY = nextY;
	msgOld->bulletPosX = nextXB;
	msgOld->bulletPosY = nextYB;
	msgOld->rotationAngle = nextR;

	PacketData returnData = *msgOld;
	lastPredictionTime = gameTime;

	return returnData;
}