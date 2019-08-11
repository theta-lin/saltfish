#include "editor.hpp"

void Editor::update(sw::Surface &surface)
{
	surface.fillRect(nullptr, {0, 0, 0, 255});
	for (const Game::Line &line : game.getLines())
	{
		Line lineDraw{game.getVertices()[line.v0], game.getVertices()[line.v1]};
		lineDraw.draw({255, 255, 255, 255}, surface);
	}

	for (size_t i{0}; i < game.getVertices().size(); ++i)
	{
		sw::Surface indexRender{font.renderBlended(std::to_string(i), {255, 130, 0, 255})};
		sw::Rect dstRect{static_cast<int>(std::roundl(game.getVertices()[i][0])),
				         static_cast<int>(std::roundl(game.getVertices()[i][1])),
		                 0, 0};
		indexRender.blit(surface, nullptr, &dstRect);
	}
}

Editor::Editor(Game &game, UI &ui, const std::filesystem::path &exeDir, std::function<void()> onExit)
	: game{game}, ui{ui}, font{exeDir / "font" / "DejaVuSansMono.ttf", 20},
	screen{{0.0, 0.0, 1.0, 0.9}, [this](sw::Surface &surface){ this->update(surface); }},
	input{{0.0, 0.9, 1.0, 0.05}, {{255, 255, 255, 255}, {40, 40, 40, 255}}, exeDir / "font" / "DejaVuSansMono.ttf"},
	message{{0.0, 0.95, 1.0, 0.05}, {{255, 130, 0, 255}, {40, 40, 40, 255}}, exeDir / "font" / "DejaVuSansMono.ttf"},
	onExit{onExit}
{
	ui.add(screen);
	ui.add(input);
	ui.add(message);
}

Editor::~Editor()
{
	ui.remove(screen);
	ui.remove(input);
	ui.remove(message);
}

void Editor::handleEvent(const sw::Event &event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		{
			switch(event.key.keysym.scancode)
			{
			case SDL_SCANCODE_BACKSPACE:
				if (input.getText().size() > 0)
					input.getText().pop_back();
				break;

			case SDL_SCANCODE_RETURN:
			{
				auto tokens{tokenize(input.getText())};
				auto it = tokens.begin();
				input.getText().clear();
				message.getText().clear();
				if (tokens.size() >= 1)
				{
					try
					{
						if (*it == "q" || *it == "quit")
						{
							onExit();
						}
						else if (*it == "n" || *it == "new")
						{
							game.free();
						}
						else if (*it == "o" || *it == "open")
						{
							std::string levelName;
							convertTokens(++it, tokens.end(), levelName);
							game.free();
							if (!game.loadLevel(levelName))
								message.getText() = "Error: game.loadLevel() failed";
						}
						else if (*it == "s" || *it == "save")
						{
							std::string levelName;
							convertTokens(++it, tokens.end(), levelName);
							if (!game.saveLevel(levelName))
								message.getText() = "Error: game.saveLevel() failed";
						}
						else if (*it == "v" || *it == "vertex")
						{
							Game::Vertex vertex;
							convertTokens(++it, tokens.end(), vertex[0], vertex[1]);
							if (!game.addVertex(vertex))
								message.getText() = "Error: game.addVertex() failed";
						}
						else if (*it == "l" || *it == "line")
						{
							Game::Line line;
							convertTokens(++it, tokens.end(), line.v0, line.v1);
							if (!game.addLine(line))
								message.getText() = "Error: game.addLine() failed";
						}
						else if (*it == "d" || *it == "delete")
						{
							if (tokens.size() >= 2)
							{
								++it;
								if (*it == "v" || *it == "vertex")
								{
									uint16_t index;
									convertTokens(++it, tokens.end(), index);
									if (!game.removeVertex(index))
										message.getText() = "Error: game.removeVertex() failed";
								}
								else if (*it == "l" || *it == "line")
								{
									uint16_t v0, v1;
									convertTokens(++it, tokens.end(), v0, v1);
									if (!game.removeLine(v0, v1))
										message.getText() = "Error: game.removeLine() failed";
								}
								else
								{
									message.getText() = "Error: delete: Command does not exist";
								}
							}
							else
							{
								message.getText() = "Error: delete: Wrong number of tokens";
							}
						}
						else
						{
							message.getText() = "Error: Command does not exist";
						}
					}
					catch (std::runtime_error &exception)
					{
						message.getText() = exception.what();
					}
				}
			}

			default:
				break;
			}
		}
		break;

	case SDL_TEXTINPUT:
		input.getText() += event.text.text;
		break;

	default:
		break;
	}
}

