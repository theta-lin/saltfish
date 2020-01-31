#include "program.hpp"

ProgramState::ProgramState(Program &program) : program{program}, ui{program.surface}, next{nullptr}
{
}

ProgramState::~ProgramState()
{
}

std::unique_ptr<ProgramState> ProgramState::handleEvent(const SDL_Event &event)
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

void MenuState::Background::draw(sw::Surface &surface)
{
	for (int col{0}; col < real.w; ++col)
	{
		for (int row{0}; row < real.h; ++row)
		{
			if ((col / 150 + row / 150) & 1)
				surface(col, row) = {0, 230, 50, 255};
			else
				surface(col, row) = {50, 150, 0, 255};
		}
	}
}

MenuState::MenuState(Program &program) : ProgramState{program}, background{{0.0, 0.0, 1.0, 1.0}}, menu{makeMainMenu({0.2, 0.4, 0.6, 0.4}, 0.1, 0.01, program.exeDir / "font")}
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
	sw::Font font{program.exeDir / "font" / "Terminus-Bold.ttf", 50};
	line1 = font.renderBlended("ESC: Pause Game", {255, 255, 255, 255});
}

std::unique_ptr<ProgramState> GameState::handleEvent(const SDL_Event &event)
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
	program.surface.fillRect(nullptr, {0, 0, 0, 255});
	sw::Rect dist1{200, 50, -1, -1};
	line1.blit(program.surface, nullptr, &dist1);
}

PauseState::PauseState(Program &program)
	: ProgramState{program}, menu{makeMainMenu({0.2, 0.4, 0.6, 0.4}, 0.1, 0.01, program.exeDir / "font")}
{

	menu.add({"Back To Game", [this](){ this->next = std::make_unique<GameState>(this->program); }});
	menu.add({"Quit To Menu", [this](){ this->next = std::make_unique<MenuState>(this->program); }});
	menu.add({"Configuration", nullptr, false});
	menu.add({"Quit Game", [this](){ this->next = std::make_unique<ExitState>(this->program); }});
	ui.add(menu);
}

EditorState::EditorState(Program &program) : ProgramState{program},
	status{{0.0, 0.9, 1.0, 0.05}, {textNormal, background}, program.exeDir / "font" / "DejaVuSansMono.ttf"},
	message{{0.0, 0.95, 1.0, 0.05}, {textHighlight, background}, program.exeDir / "font" / "DejaVuSansMono.ttf"},
	editor{{0.0, 0.0, 1.0, 0.9}, program.game, status.text, message.text, [this](){ this->next = std::make_unique<MenuState>(this->program); }}
{
}

std::unique_ptr<ProgramState> EditorState::handleEvent(const SDL_Event &event)
{
	editor.handleEvent(event);
	return ProgramState::handleEvent(event);
}

ExitState::ExitState(Program &program) : ProgramState{program}
{
}

std::unique_ptr<ProgramState> ExitState::handleEvent([[maybe_unused]] const SDL_Event &event)
{
	return nullptr;
}

void ExitState::update()
{
	return;
}

Program::Program(Log &logger, const fs::path &exeDir, sw::Surface &surface)
	: state{std::make_unique<MenuState>(*this)}, logger{logger}, exeDir{exeDir}, surface{surface}, game{logger, exeDir}
{
}

void Program::handleEvent(const SDL_Event &event)
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

