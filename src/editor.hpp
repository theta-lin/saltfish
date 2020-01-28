#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <cmath>
#include "game.hpp"
#include "ui.hpp"
#include "line.hpp"
#include "io.hpp"

/*
 * Quick hacks to display a map editor
 * TODO: REPLACE ALL OF THESE WITH A PROPER EDITOR
 */
class Editor
{
private:
	Game &game;
	UI &ui;
	sw::Font font;
	DrawableWidget screen;
	TextBar input;
	TextBar message;
	std::function<void()> onExit;

	void update(sw::Surface &surface);

public:
	Editor(Game &game, UI &ui, const std::filesystem::path &exeDir, std::function<void()> onExit);
	~Editor();
	void handleEvent(const sw::Event &event);
};

#endif // ifndef EDITOR_HPP

