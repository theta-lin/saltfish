#ifndef GAME_HPP
#define GAME_HPP

#include "level.hpp"

class Game
{
public:
	Game(Log &logger, const std::filesystem::path &exeDir);
	Log &logger;
	Level level;
};

#endif // ifndef GAME_HPP

