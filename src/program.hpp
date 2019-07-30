#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>
#include <memory>
#include <filesystem>
#include <SDL.h>
#include "log.hpp"
#include "surface.hpp"
#include "font.hpp"

namespace fs = std::filesystem;

class Program;

class ProgramState
{
protected:
	Program &program;

public:
	ProgramState(Program &program);
	virtual ~ProgramState();
	virtual std::unique_ptr<ProgramState> handleInput(const SDL_Event &event) = 0;
	virtual void update() = 0;
};

class MenuState : public ProgramState
{
private:
	Surface line1;
	Surface line2;
	Surface line3;

public:
	MenuState(Program &program);
	std::unique_ptr<ProgramState> handleInput(const SDL_Event &event) override;
	void update() override;
};

class GameState : public ProgramState
{
private:
	Surface line1;

public:
	GameState(Program &program);
	std::unique_ptr<ProgramState> handleInput(const SDL_Event &event) override;
	void update() override;
};

class PauseState : public ProgramState
{
private:
	Surface line1;
	Surface line2;
	Surface line3;

public:
	PauseState(Program &program);
	std::unique_ptr<ProgramState> handleInput(const SDL_Event &event) override;
	void update() override;
};

class EditorState : public ProgramState
{
private:
	Surface line1;

public:
	EditorState(Program &program);
	std::unique_ptr<ProgramState> handleInput(const SDL_Event &event) override;
	void update() override;
};

class ExitState : public ProgramState
{
public:
	ExitState(Program &program);
	std::unique_ptr<ProgramState> handleInput([[maybe_unused]] const SDL_Event &event) override;
	void update() override;
};

class Program
{
private:
	Log &logger;
	const fs::path &exeDir;
	Surface &surface;
	std::unique_ptr<ProgramState> state;

public:
	Program(Log &logger, const fs::path &exeDir, Surface &surface);
	void handleInput(const SDL_Event &event);
	void update();
	bool isExited();

	Log& getLog();
	Surface& getSurface();
	const fs::path& getExeDir();
};

#endif // ifndef PROGRAM_HPP

