#include "Main.h"

namespace {
	std::unique_ptr<Game> game;
}

void parse_config_file(const std::string& filename, std::string& ip, int& port) {
	std::ifstream file(filename);
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string key;
		if (std::getline(iss, key, '=')) {
			std::string value;
			if (std::getline(iss, value)) {
				// Do something with the key-value pair.
				if (key == "ip") {
					ip = value;
				} else if (key == "port") {
					port = stoi(value);
				}
			}
		}
	}
}

int main() {
	std::string ip = {};
	int port = 0;
	parse_config_file("net_config.txt", ip, port);

	std::cout << "Starting game..." << std::endl;
	game = std::make_unique<Game>(ip, port);
	game->InitializeGame();
}