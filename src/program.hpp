/*
 * This file specifies the main program state machine,
 * each state will return its next state by handleEvent().
 * terminating the program is through the special ExitState.
 * Each state should hook its UI widgets,
 * all event (except the SDL_QUIT event) is handled by the UI,
 */

#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "editor.hpp"
#include "game.hpp"
#include "ui.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace fs = std::filesystem;

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

	/*
	 * Return the next state after handling events,
	 * nullptr if keep current state.
	 */
	// TODO: Remove virtual when GameState is completed
	virtual std::unique_ptr<ProgramState> handleEvent(const SDL_Event &event);
	virtual void update();
};

class MenuState : public ProgramState
{
private:
	/*
	 * Temporary background for TESTING
	 * TODO: Replace the background
	 */
	class Background: public Widget
	{
	public:
		using Widget::Widget;
		void draw(sw::Surface &surface) final;
	};
	Background background;

	Menu menu;

public:
	MenuState(Program &program);
};

// TODO: Finish GameState (currently a placeholder)
class GameState : public ProgramState
{
private:
	sw::Surface line1;

public:
	GameState(Program &program);
	std::unique_ptr<ProgramState> handleEvent(const SDL_Event &event) override;
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
	const sw::Color textNormal{255, 255, 255, 255};
	const sw::Color textHighlight{255, 130, 0, 255};
	const sw::Color background{40, 40, 40, 255};

	TextBar status;
	TextBar message;
	Editor editor;

public:
	EditorState(Program &program);
};

class ExitState : public ProgramState
{
public:
	ExitState(Program &program);
	std::unique_ptr<ProgramState> handleEvent([[maybe_unused]] const SDL_Event &event) override;
	void update() override;
};

class Program
{
public:
	Log &logger;
	const fs::path &exeDir;
	sw::Window &window;
	Game game;

private:
	std::unique_ptr<ProgramState> state;

public:
	Program(Log &logger, const fs::path &exeDir, sw::Window &window);
	void handleEvent(const SDL_Event &event);
	void update();
	bool isExited();
};

#endif // ifndef PROGRAM_HPP

