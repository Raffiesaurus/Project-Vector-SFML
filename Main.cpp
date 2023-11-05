#include "Main.h"

namespace {
	std::unique_ptr<Game> g_game;
}

int main() {
	std::cout << "Starting game..." << std::endl;

	g_game = std::make_unique<Game>();

	g_game->InitializeGame();

	return 0;
}