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
	udpSocket.setBlocking(false);
	TcpSocketStatus = tcpSocket.connect(serverIp, serverPort);
	if (TcpSocketStatus != sf::Socket::Done) {
		// error...
		std::cout << "Error " << TcpSocketStatus;
		return false;
	} else {
		std::cout << "Connected\n";
		if (udpSocket.bind(tcpSocket.getLocalPort()) != sf::Socket::Done) {
			std::cerr << "Error binding UDP socket to port " << tcpSocket.getLocalPort() << std::endl;
			return 1;
		} else {
			std::cout << "UDP Listening with port " << tcpSocket.getLocalPort() << std::endl;
		}
		std::cout << "\nMy ports : " << tcpSocket.getLocalPort() << " " << udpSocket.getLocalPort() << " ";
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

void NetworkManager::SendPositionData(PositionData data) {
	std::cout << "\nData Sending: " << data.x << "  " << data.y;
	sf::Packet packet;
	packet << data.x << data.y;
	//if (udpSocket.send(packet, serverIp, serverPort) != sf::Socket::Done) {
	//	//std::cout << "\nError Sending";
	//} else {
	//	std::cout << "\nData Sent";
	//}
	udpSocket.send(packet, serverIp, serverPort);
	return;
}

NetworkManager::PositionData NetworkManager::GetPositionData() {
	sf::Packet packet;
	PositionData data{};
	data.x = data.y = 100000;

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
	std::cout << "\nData Received: ";
	packet >> data.x >> data.y;
	std::cout << data.x << "  " << data.y;
	return data;
}