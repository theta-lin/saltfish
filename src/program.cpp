#include "program.hpp"

ProgramState::ProgramState(Program &program) : program{program}
{
}

ProgramState::~ProgramState()
{
}

MenuState::MenuState(Program &program) : ProgramState{program}
{
	Font font{program.getExeDir() / "font" / "DejaVuSansMono.ttf", 50};
	line1 = font.renderBlended("1: Start Game", {255, 255, 255, 255});
	line2 = font.renderBlended("2: Start Editor", {255, 255, 255, 255});
	line3 = font.renderBlended("3: Quit Game", {255, 255, 255, 255});
}

std::unique_ptr<ProgramState> MenuState::handleInput(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_QUIT:
		return std::make_unique<ExitState>(program);

	case SDL_KEYDOWN:
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_1:
			return std::make_unique<GameState>(program);
		case SDLK_2:
			return std::make_unique<EditorState>(program);
		case SDLK_3:
			return std::make_unique<ExitState>(program);
		default:
			return nullptr;
		}
	}

	default:
		return nullptr;
	}
}

void MenuState::update()
{
	program.getSurface().fillRect(nullptr, {0, 0, 0, 255});
	SDL_Rect dist1{200, 50, -1, -1};
	line1.blitSurface(program.getSurface(), nullptr, &dist1);
	SDL_Rect dist2{200, 110, -1, -1};
	line2.blitSurface(program.getSurface(), nullptr, &dist2);
	SDL_Rect dist3{200, 170, -1, -1};
	line3.blitSurface(program.getSurface(), nullptr, &dist3);
}

GameState::GameState(Program &program) : ProgramState{program}
{
	Font font{program.getExeDir() / "font" / "DejaVuSansMono.ttf", 50};
	line1 = font.renderBlended("1: Pause Game", {255, 255, 255, 255});
}

std::unique_ptr<ProgramState> GameState::handleInput(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_QUIT:
		return std::make_unique<ExitState>(program);

	case SDL_KEYDOWN:
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_1:
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
	SDL_Rect dist1{200, 50, -1, -1};
	line1.blitSurface(program.getSurface(), nullptr, &dist1);
}

PauseState::PauseState(Program &program) : ProgramState{program}
{
	Font font{program.getExeDir() / "font" / "DejaVuSansMono.ttf", 50};
	line1 = font.renderBlended("1: Back To Game", {255, 255, 255, 255});
	line2 = font.renderBlended("2: Quit To Menu", {255, 255, 255, 255});
	line3 = font.renderBlended("3: Quit Game", {255, 255, 255, 255});
}

std::unique_ptr<ProgramState> PauseState::handleInput(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_QUIT:
		return std::make_unique<ExitState>(program);

	case SDL_KEYDOWN:
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_1:
			return std::make_unique<GameState>(program);
		case SDLK_2:
			return std::make_unique<MenuState>(program);
		case SDLK_3:
			return std::make_unique<ExitState>(program);
		default:
			return nullptr;
		}
	}

	default:
		return nullptr;
	}
}

void PauseState::update()
{
	program.getSurface().fillRect(nullptr, {0, 0, 0, 255});
	SDL_Rect dist1{200, 50, -1, -1};
	line1.blitSurface(program.getSurface(), nullptr, &dist1);
	SDL_Rect dist2{200, 110, -1, -1};
	line2.blitSurface(program.getSurface(), nullptr, &dist2);
	SDL_Rect dist3{200, 170, -1, -1};
	line3.blitSurface(program.getSurface(), nullptr, &dist3);
}


EditorState::EditorState(Program &program) : ProgramState{program}
{
	Font font{program.getExeDir() / "font" / "DejaVuSansMono.ttf", 50};
	line1 = font.renderBlended("1: Quit To Menu", {255, 255, 255, 255});
}

std::unique_ptr<ProgramState> EditorState::handleInput(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_QUIT:
		return std::make_unique<ExitState>(program);

	case SDL_KEYDOWN:
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_1:
			return std::make_unique<MenuState>(program);
		default:
			return nullptr;
		}
	}

	default:
		return nullptr;
	}
}

void EditorState::update()
{
	program.getSurface().fillRect(nullptr, {0, 0, 0, 255});
	SDL_Rect dist1{200, 50, -1, -1};
	line1.blitSurface(program.getSurface(), nullptr, &dist1);
}

ExitState::ExitState(Program &program) : ProgramState{program}
{
}

std::unique_ptr<ProgramState> ExitState::handleInput([[maybe_unused]] const SDL_Event &event)
{
	return nullptr;
}

void ExitState::update()
{
	return;
}

Program::Program(Log &logger, const fs::path &exeDir, Surface &surface)
	: logger{logger}, exeDir{exeDir}, surface{surface}, state{std::make_unique<MenuState>(*this)}
{
}

void Program::handleInput(const SDL_Event &event)
{
	std::unique_ptr<ProgramState> nextState{state->handleInput(event)};
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

Surface& Program::getSurface()
{
	return surface;
}

const fs::path& Program::getExeDir()
{
	return exeDir;
}

