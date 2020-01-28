#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "level.hpp"

class Game
{
private:
	Log &logger;
	Level level;

public:
	Game(Log &logger, const std::filesystem::path &exeDir);
	Level& getLevel();
};

#endif // ifndef LEVEL_HPP

