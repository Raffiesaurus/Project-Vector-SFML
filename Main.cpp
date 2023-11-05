#include "Main.h"

namespace {
	std::unique_ptr<Game> game;
}

int main() {
	std::cout << "Starting game..." << std::endl;
	game = std::make_unique<Game>();
	game->InitializeGame();
}