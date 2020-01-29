#include "program.hpp"

ProgramState::ProgramState(Program &program) : program{program}, ui{program.getSurface()}, next{nullptr}
{
}

ProgramState::~ProgramState()
{
}

std::unique_ptr<ProgramState> ProgramState::handleEvent(const sw::Event &event)
{
	if (event.type == SDL_QUIT)
		next = std::make_unique<ExitState>(program);
	else
		ui.handleEvent(event);
	return std::move(next);
}

void ProgramState::update()
{
	ui.update();
}

MenuState::MenuState(Program &program) : ProgramState{program}, background{{0.0, 0.0, 1.0, 1.0}, drawBackground}, menu{makeMainMenu({0.2, 0.4, 0.6, 0.4}, 0.1, 0.01, program.getExeDir() / "font")}
{
	ui.add(background);
	menu.add({"Start Game", [this](){ this->next = std::make_unique<GameState>(this->program); }});
	menu.add({"Start Editor", [this](){ this->next = std::make_unique<EditorState>(this->program); }});
	menu.add({"Configuration", nullptr, false});
	menu.add({"Quit Game", [this](){ this->next = std::make_unique<ExitState>(this->program); }});
	ui.add(menu);
}

GameState::GameState(Program &program) : ProgramState{program}
{
	sw::Font font{program.getExeDir() / "font" / "Terminus-Bold.ttf", 50};
	line1 = font.renderBlended("ESC: Pause Game", {255, 255, 255, 255});
}

std::unique_ptr<ProgramState> GameState::handleEvent(const sw::Event &event)
{
	switch (event.type)
	{
	case SDL_QUIT:
		return std::make_unique<ExitState>(program);

	case SDL_KEYDOWN:
	{
		switch (event.key.keysym.scancode)
		{
		case SDL_SCANCODE_ESCAPE:
			return std::make_unique<PauseState>(program);
		default:
			return nullptr;
		}
	}

	default:
		return nullptr;
	}
}

void GameState::update()
{
	program.getSurface().fillRect(nullptr, {0, 0, 0, 255});
	sw::Rect dist1{200, 50, -1, -1};
	line1.blit(program.getSurface(), nullptr, &dist1);
}

PauseState::PauseState(Program &program)
	: ProgramState{program}, menu{makeMainMenu({0.2, 0.4, 0.6, 0.4}, 0.1, 0.01, program.getExeDir() / "font")}
{

	menu.add({"Back To Game", [this](){ this->next = std::make_unique<GameState>(this->program); }});
	menu.add({"Quit To Menu", [this](){ this->next = std::make_unique<MenuState>(this->program); }});
	menu.add({"Configuration", nullptr, false});
	menu.add({"Quit Game", [this](){ this->next = std::make_unique<ExitState>(this->program); }});
	ui.add(menu);
}

EditorState::EditorState(Program &program) : ProgramState{program}, editor{program.getGame(), ui, program.getExeDir(), [this](){ this->next = std::make_unique<MenuState>(this->program); }}
{
}

std::unique_ptr<ProgramState> EditorState::handleEvent(const sw::Event &event)
{
	editor.handleEvent(event);
	return ProgramState::handleEvent(event);
}

ExitState::ExitState(Program &program) : ProgramState{program}
{
}

std::unique_ptr<ProgramState> ExitState::handleEvent([[maybe_unused]] const sw::Event &event)
{
	return nullptr;
}

void ExitState::update()
{
	return;
}

Program::Program(Log &logger, const fs::path &exeDir, sw::Surface &surface)
	: logger{logger}, exeDir{exeDir}, surface{surface}, game{logger, exeDir}, state{std::make_unique<MenuState>(*this)}
{
}

void Program::handleEvent(const sw::Event &event)
{
	std::unique_ptr<ProgramState> nextState{state->handleEvent(event)};
	if (nextState)
		state = std::move(nextState);
}

void Program::update()
{
	state->update();
}

bool Program::isExited()
{
	return dynamic_cast<ExitState*>(state.get()) != nullptr;
}

Log& Program::getLog()
{
	return logger;
}

sw::Surface& Program::getSurface()
{
	return surface;
}

const fs::path& Program::getExeDir()
{
	return exeDir;
}

Game& Program::getGame()
{
	return game;
}

