/*
 * This file specifies the main program state machine
 */

#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>
#include <memory>
#include <filesystem>
#include "log.hpp"
#include "surface.hpp"
#include "font.hpp"
#include "event.hpp"
#include "ui.hpp"
#include "game.hpp"
#include "editor.hpp"

namespace fs = std::filesystem;

/*
 * Helper to construct a menu in default style
 */
Menu makeMenuDefault(const doubleRect &dimension, double itemHeight, double gapHeight, const std::filesystem::path &exeDir);
void drawBackground(sw::Surface &surface);

class Program;

class ProgramState
{
protected:
	Program &program;
	UI ui;
	std::unique_ptr<ProgramState> next;
	ProgramState(Program &program);

public:
	virtual ~ProgramState();
	virtual std::unique_ptr<ProgramState> handleEvent(const sw::Event &event);
	virtual void update();
};

class MenuState : public ProgramState
{
private:
	DrawableWidget background;
	Menu menu;

public:
	MenuState(Program &program);
};

class GameState : public ProgramState
{
private:
	sw::Surface line1;

public:
	GameState(Program &program);
	std::unique_ptr<ProgramState> handleEvent(const sw::Event &event) override;
	void update() override;
};

class PauseState : public ProgramState
{
private:
	Menu menu;

public:
	PauseState(Program &program);
};

class EditorState : public ProgramState
{
private:
	Editor editor;

public:
	EditorState(Program &program);
	std::unique_ptr<ProgramState> handleEvent(const sw::Event &event) override;
};

class ExitState : public ProgramState
{
public:
	ExitState(Program &program);
	std::unique_ptr<ProgramState> handleEvent([[maybe_unused]] const sw::Event &event) override;
	void update() override;
};

class Program
{
private:
	Log &logger;
	const fs::path &exeDir;
	sw::Surface &surface;

	Game game;

	std::unique_ptr<ProgramState> state;

public:
	Program(Log &logger, const fs::path &exeDir, sw::Surface &surface);
	void handleEvent(const sw::Event &event);
	void update();
	bool isExited();

	Log& getLog();
	sw::Surface& getSurface();
	const fs::path& getExeDir();
	Game& getGame();
};

#endif // ifndef PROGRAM_HPP

