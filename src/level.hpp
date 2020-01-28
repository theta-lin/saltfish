#ifndef GAME_HPP
#define GAME_HPP

#include "io.hpp"
#include "log.hpp"
#include "vec.hpp"
#include <fstream>

/*
 * A class to store objects of a game level,
 * which can be loaded/saved to a compact file format.
 *
 * Level File Format:
 * Header    | uint32_t offset to directory
 *
 * Items     | vertices
 *           | lines
 *           | ...
 *
 * Directory | uint32_t offset to the end of the item
 */
class Level
{
public:
	using Vertex = Vec2d;

	// A line is defined by two vertices
	struct Line
	{
		uint16_t v0;
		uint16_t v1;
	};

private:
	Log &logger;
	const std::filesystem::path &exeDir;

	std::vector<Vertex> vertices;
	std::list<Line> lines;

public:
	Level(Log &logger, const std::filesystem::path &exeDir);
	bool loadLevel(const std::string &levelName);
	bool saveLevel(const std::string &levelName);
	void free();

	const std::vector<Vertex>& getVertices();
	const std::list<Line>& getLines();
	bool addVertex(const Vertex &vertex);
	bool addLine(const Line &line);
	bool removeVertex(uint16_t index);
	bool removeLine(uint16_t v0, uint16_t v1);
};

#endif // ifndef GAME_HPP

