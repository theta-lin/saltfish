#include "game.hpp"

Game::Game(Log &logger, const std::filesystem::path &exeDir)
	: logger{logger}, exeDir{exeDir}
{
}

bool Game::loadLevel(const std::string &levelName)
{
	auto levelPath{exeDir / "level" / levelName};
	std::ifstream ifs{levelPath, std::ios::binary | std::ios::ate};
	if (!ifs)
	{
		logger.lock();
		logger.GET(Log::warning) << "Game::loadLevel() failed: Cannot open file \"" << levelPath.string() << '\"' << std::endl;
		logger.unlock();
		return false;
	}

	auto end{ifs.tellg()};
	ifs.seekg(0, std::ios::beg);
	auto size{static_cast<std::size_t>(end - ifs.tellg())};
	if (size == 0)
	{
		logger.lock();
		logger.GET(Log::warning) << "Game::loadLevel() failed: File \"" << levelPath.string() << "\" is empty" << std::endl;
		logger.unlock();
		return false;
	}

	std::vector<std::byte> buffer(size);
	if (!ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size()))
	{
		logger.lock();
		logger.GET(Log::warning) << "Game::loadLevel() failed: Failed to read from file \"" << levelPath.string() << '\"' << std::endl;
		logger.unlock();
		return false;
	}

	try
	{
		std::size_t index{0};
		uint32_t directoryOffset;
		deserial(directoryOffset, buffer, index);

		std::size_t directoryIndex{static_cast<std::size_t>(directoryOffset)};
		uint32_t verticesEnd;
		deserial(verticesEnd, buffer, directoryIndex);
		uint32_t linesEnd;
		deserial(linesEnd, buffer, directoryIndex);

		while (index < std::size_t(verticesEnd))
		{
			double x;
			deserial(x, buffer, index);
			double y;
			deserial(y, buffer, index);
			vertices.push_back({x, y});
		}

		while(index < std::size_t(linesEnd))
		{
			uint16_t v0;
			deserial(v0, buffer, index);
			uint16_t v1;
			deserial(v1, buffer, index);
			lines.push_back({v0, v1});
		}
	}
	catch (std::out_of_range &exception)
	{
		logger.lock();
		logger.GET(Log::warning) << "Game::loadLevel() failed: Caught std::out_of_range: " << exception.what() << "; when parsing file \"" << levelPath.string() << '\"' << std::endl;
		logger.unlock();
		return false;
	}

	logger.lock();
	logger.GET(Log::info) << "Game::loadLevel(): successfully loaded \"" << levelPath.string() << '\"' << std::endl;
	logger.unlock();

	return true;
}

bool Game::saveLevel(const std::string &levelName)
{
	std::vector<std::byte> header;
	std::vector<std::byte> items;
	std::vector<std::byte> directory;

	for (const Vertex &vertex : vertices)
	{
		serial(vertex[0], items);
		serial(vertex[1], items);
	}
	serial(static_cast<uint32_t>(4 + items.size()), directory);

	for (const Line &line : lines)
	{
		serial(line.v0, items);
		serial(line.v1, items);
	}
	serial(static_cast<uint32_t>(4 + items.size()), directory);

	serial(static_cast<uint32_t>(4 + items.size()), header);

	if (!std::filesystem::exists(exeDir / "level"))
	{
		if (!std::filesystem::create_directory(exeDir / "level"))
		{
			logger.lock();
			logger.GET(Log::warning) << "Game::saveLevel() failed: Cannot create directory \"" << std::filesystem::path(exeDir / "level").string() << '\"' << std::endl;
			logger.unlock();
		}
	}

	auto levelPath{exeDir / "level" / levelName};
	std::ofstream ofs{levelPath, std::ios::binary};
	if (!ofs)
	{
		logger.lock();
		logger.GET(Log::warning) << "Game::saveLevel() failed: Cannot open file \"" << levelPath.string() << '\"' << std::endl;
		logger.unlock();
		return false;
	}

	if (!ofs.write(reinterpret_cast<char*>(header.data()), header.size()))
		goto writeFail;
	if (!ofs.write(reinterpret_cast<char*>(items.data()), items.size()))
		goto writeFail;
	if (!ofs.write(reinterpret_cast<char*>(directory.data()), directory.size()))
		goto writeFail;

	logger.lock();
	logger.GET(Log::info) << "Game::saveLevel(): successfully saved \"" << levelPath.string() << '\"' << std::endl;
	logger.unlock();
	return true;

writeFail:
	logger.lock();
	logger.GET(Log::warning) << "Game::saveLevel() failed: Failed write to file \"" << levelPath.string() << '\"' << std::endl;
	logger.unlock();
	return false;
}

void Game::free()
{
	vertices.clear();
	lines.clear();
}

const std::vector<Game::Vertex>& Game::getVertices()
{
	return vertices;
}

const std::list<Game::Line>& Game::getLines()
{
	return lines;
}

bool Game::addVertex(const Vertex &vertex)
{
	vertices.push_back(vertex);
	return true;
}

bool Game::addLine(const Line &line)
{
	if (line.v0 == line.v1)
		return false;

	Line temp{line};
	if (temp.v0 > temp.v1)
		std::swap(temp.v0, temp.v1);
	for (const Line &l : lines)
	{
		if (l.v0 == temp.v0 && l.v1 == temp.v1)
			return false;
	}

	lines.push_back(temp);
	return true;
}

bool Game::removeVertex(uint16_t index)
{
	if (index < 0 || index >= vertices.size())
		return false;

	vertices.erase(vertices.begin() + index);
	for (auto it{lines.begin()}; it != lines.end(); )
	{
		if (it->v0 == index || it->v1 == index)
		{
			it = lines.erase(it);
		}
		else
		{
			if (it->v0 > index) --it->v0;
			if (it->v1 > index) --it->v1;
			++it;
		}
	}

	return true;
}

bool Game::removeLine(uint16_t v0, uint16_t v1)
{
	for (auto it{lines.begin()}; it != lines.end(); )
	{
		if (it->v0 == v0 && it->v1 == v1)
		{
			lines.erase(it);
			return true;
		}
		else
		{
			++it;
		}
	}

	return false;
}

void Game::useTestData()
{
	vertices.push_back({1.0, 1.0});
	vertices.push_back({2.01, 3.001});
	vertices.push_back({999999.0, 999.01});
	lines.push_back({0, 2});
	lines.push_back({1, 0});
	lines.push_back({2, 1});
}

void Game::print()
{
	logger.lock();
	logger.GET(Log::debug) << "Vertices:" << std::endl;
	for (const Vertex &vertex : vertices)
		logger.GET(Log::debug) << '\t' << '(' << vertex[0] << ',' << vertex[1] << ')' << std::endl;
	logger.GET(Log::debug) << "Lines:" << std::endl;
	for (const Line &line : lines)
		logger.GET(Log::debug) << '\t' << '(' << line.v0 << ',' << line.v1 << ')' << std::endl;
	logger.unlock();
}

