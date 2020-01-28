#include "game.hpp"

Game::Game(Log &logger, const std::filesystem::path &exeDir)
	: logger{logger}, level{logger, exeDir}
{
}

Level& Game::getLevel()
{
	return level;
}

