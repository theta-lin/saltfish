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
 * Header    uint32_t: directoryOffset ----+
 *                                         |
 * Items     Vertices                      |
 *           | double: x                   |
 *           | double: y                   |
 *           Lines         <--------+      |
 *           | uint16_t: v0         |      |
 *           | uint16_t: v1         |      |
 *                         <--------+-+    |
 * Directory uint32_t: verticesEnd -+ | <--+
 *           uint32_t: linesEnd ------+
 */
class Level
{
public:
	using Vertex = Vec2d;

	// A line is defined by the ID of two vertices
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
	bool load(const std::string &levelName);
	bool save(const std::string &levelName);
	void free();

	const std::vector<Vertex>& getVertices();
	const std::list<Line>& getLines();
	bool addVertex(const Vertex &vertex);
	bool addLine(const Line &line);

	// NOTE: This also REMOVE all the lines CONNECTED TO IT,
	//       and DECREASES the ID of vertices with ID GREATER than this vertex.
	bool removeVertex(uint16_t index);
	bool removeLine(uint16_t v0, uint16_t v1);
};

#endif // ifndef GAME_HPP

