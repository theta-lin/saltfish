#include "editor.hpp"

Editor::Tool::Tool(Editor &editor) : editor{editor}
{
}

Editor::Tool::~Tool()
{
}

Editor::NullTool::NullTool(Editor &editor) : Tool{editor}, confirmQuit{false}
{
	editor.status = defaultStatus + defaultStatusPrompt;
}

std::pair<bool, std::unique_ptr<Editor::Tool> > Editor::NullTool::handleEvent(const SDL_Event &event, const Vec2d &coord)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_q:
			confirmQuit = true;
			editor.status = quitPromptStatus;
			break;

		case SDLK_y:
			if (confirmQuit)
				editor.onExit();
			break;

		case SDLK_n:
			[[fallthrough]];
		case SDLK_ESCAPE:
			[[fallthrough]];
		case SDLK_RETURN:
			if (confirmQuit)
			{
				confirmQuit = false;
				editor.status = defaultStatus;
			}
			break;

		default:
			break;
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		[[fallthrough]];
	case SDL_MOUSEMOTION:
		if (!confirmQuit)
			editor.status = defaultStatus + '(' + std::to_string(coord[0]) + ',' + std::to_string(coord[1]) + ')';
		break;

	default:
		break;
	}

	return {false, nullptr};
}

void Editor::NullTool::activate()
{
}

void Editor::NullTool::undo()
{
}

Editor::History::History() : current{operations.end()}
{
}

void Editor::History::append(std::unique_ptr<Tool> tool)
{
	operations.erase(current, operations.end());
	operations.push_back(std::move(tool));
	current = operations.end();
}

bool Editor::History::undo()
{
	if (current == operations.begin())
		return false;
	--current;
	(*current)->undo();
	return true;
}

bool Editor::History::redo()
{
	if (current == operations.end())
		return false;
	(*current)->activate();
	++current;
	return true;
}

void Editor::toolHandleEvent(const SDL_Event &event, const Vec2d &coord)
{
	auto ret{tool->handleEvent(event, coord)};
	if (ret.second)
	{
		if (ret.first)
		{
			history.append(std::move(tool));
		}
		tool = std::move(ret.second);
	}
}

Editor::Editor(const DoubleRect &dimension, sw::Window &window, Game &game, Log &logger, std::string &status, std::string &message, std::function<void()> onExit)
	: Widget{dimension}, view{{0.0, 0.0}, 0.005},
	  window{window}, game{game}, tool{std::make_unique<NullTool>(*this)},
	  hLocked{false}, yAlign{0}, vLocked{false}, xAlign{0},
	  leftMouseButtonDown{false}, dragged{false}, xOrigin{0}, yOrigin{0},
	  logger{logger}, status{status}, message{message}, onExit{onExit}
{

}

void Editor::handleEvent(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			tool = std::make_unique<NullTool>(*this);
			break;

		case SDLK_u:
			tool = std::make_unique<NullTool>(*this);
			if (history.undo())
				message = "Undo one operation";
			else
				message = "Already at oldest change";
			break;

		case SDLK_r:
			tool = std::make_unique<NullTool>(*this);
			if (history.redo())
				message = "Redo one operation";
			else
				message = "Already at newest change";
			break;

		case SDLK_LSHIFT:
			hLocked = !hLocked;
			if (hLocked)
				message = "Horizontal locked";
			else
				message = "Horizontal unlocked";
			vLocked = false;
			break;

		case SDLK_LALT:
			vLocked = !vLocked;
			if (vLocked)
				message = "Vertical locked";
			else
				message = "Vertical unlocked";
			hLocked = false;
			break;

		default:
			toolHandleEvent(event, {});
			break;
		}
		break;

	case SDL_MOUSEMOTION:
	{
		SDL_Event modified{event};
		if (hLocked)
		{
			SDL_WarpMouseInWindow(window.getPtr(), event.motion.x, yAlign);
			modified.motion.y = yAlign;
		}
		else
		{
			yAlign = event.motion.y;
		}

		if (vLocked)
		{
			SDL_WarpMouseInWindow(window.getPtr(), xAlign, event.motion.y);
			modified.motion.x = xAlign;
		}
		else
		{
			xAlign = event.motion.x;
		}

		if (leftMouseButtonDown)
		{
			dragged = true;

			// Here we are dragging the view OPPOSITE of mouse's motion,
			// because we actually want to drag the CANVAS not the VIEW
			view.origin[0] += (xOrigin - modified.motion.x) * view.scale;
			view.origin[1] += (yOrigin - modified.motion.y) * view.scale;
			message = "Dragging";
		}
		else
		{
			Vec2d coord{static_cast<double>(modified.motion.x), static_cast<double>(modified.motion.y)};
			coord *= view.scale;
			coord += view.origin;
			toolHandleEvent(modified, coord);
		}

		xOrigin = modified.motion.x;
		yOrigin = modified.motion.y;
		break;
	}

	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT)
			leftMouseButtonDown = true;
		break;

	case SDL_MOUSEBUTTONUP:
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			if (!dragged)
			{
				Vec2d coord{static_cast<double>(event.button.x), static_cast<double>(event.button.y)};
				coord *= view.scale;
				coord += view.origin;
				toolHandleEvent(event, coord);
			}
			leftMouseButtonDown = false;
			dragged = false;
		}
		break;

	case SDL_MOUSEWHEEL:
	{
		double zoom{zoomCoeff * event.wheel.y};
		if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			zoom = -zoom;
		view.scale = std::clamp(view.scale + zoom, zoomMin, zoomMax);
		break;
	}
	}
}

void Editor::draw(sw::Surface &surface)
{
	surface.fillRect(&real, backgroundColor);
	auto vertices{game.level.getVertices()};
	for (const Level::Line &line : game.level.getLines())
	{
		LineShape lineDraw{
		                  	(vertices[line.v0] - view.origin) / view.scale,
							(vertices[line.v1] - view.origin) / view.scale
		                  };
		lineDraw.draw(foregroundColor, surface);
	}
}

/*
 *void Editor::Canvas::onDraw(sw::Surface &surface)
 *{
 *    surface.fillRect(nullptr, {0, 0, 0, 255});
 *    for (const Level::Line &line : level.getLines())
 *    {
 *        LineShape lineDraw{level.getVertices()[line.v0], level.getVertices()[line.v1]};
 *        lineDraw.draw({255, 255, 255, 255}, surface);
 *    }
 *
 *    for (size_t i{0}; i < level.getVertices().size(); ++i)
 *    {
 *        sw::Surface indexRender{font.renderBlended(std::to_string(i), {255, 130, 0, 255})};
 *        sw::Rect dstRect{static_cast<int>(std::roundl(level.getVertices()[i][0])),
 *                         static_cast<int>(std::roundl(level.getVertices()[i][1])),
 *                         0, 0};
 *        indexRender.blit(surface, nullptr, &dstRect);
 *    }
 *}
 */

/*
 *void Editor::handleEvent(const SDL_Event &event)
 *{
 *    switch (event.type)
 *    {
 *    case SDL_KEYDOWN:
 *        {
 *            switch(event.key.keysym.scancode)
 *            {
 *            case SDL_SCANCODE_BACKSPACE:
 *                if (input.getText().size() > 0)
 *                    input.getText().pop_back();
 *                break;
 *
 *            case SDL_SCANCODE_RETURN:
 *            {
 *                auto tokens{tokenize(input.getText())};
 *                auto it = tokens.begin();
 *                input.getText().clear();
 *                message.getText().clear();
 *                if (tokens.size() >= 1)
 *                {
 *                    try
 *                    {
 *                        if (*it == "q" || *it == "quit")
 *                        {
 *                            onExit();
 *                        }
 *                        else if (*it == "n" || *it == "new")
 *                        {
 *                            level.free();
 *                        }
 *                        else if (*it == "o" || *it == "open")
 *                        {
 *                            std::string levelName;
 *                            convertTokens(++it, tokens.end(), levelName);
 *                            level.free();
 *                            if (!level.load(levelName))
 *                                message.getText() = "Error: level.load() failed";
 *                        }
 *                        else if (*it == "s" || *it == "save")
 *                        {
 *                            std::string levelName;
 *                            convertTokens(++it, tokens.end(), levelName);
 *                            if (!level.save(levelName))
 *                                message.getText() = "Error: level.save() failed";
 *                        }
 *                        else if (*it == "v" || *it == "vertex")
 *                        {
 *                            Level::Vertex vertex;
 *                            convertTokens(++it, tokens.end(), vertex[0], vertex[1]);
 *                            if (!level.addVertex(vertex))
 *                                message.getText() = "Error: level.addVertex() failed";
 *                        }
 *                        else if (*it == "l" || *it == "line")
 *                        {
 *                            Level::Line line;
 *                            convertTokens(++it, tokens.end(), line.v0, line.v1);
 *                            if (!level.addLine(line))
 *                                message.getText() = "Error: level.addLine() failed";
 *                        }
 *                        else if (*it == "d" || *it == "delete")
 *                        {
 *                            if (tokens.size() >= 2)
 *                            {
 *                                ++it;
 *                                if (*it == "v" || *it == "vertex")
 *                                {
 *                                    uint16_t index;
 *                                    convertTokens(++it, tokens.end(), index);
 *                                    if (!level.removeVertex(index))
 *                                        message.getText() = "Error: level.removeVertex() failed";
 *                                }
 *                                else if (*it == "l" || *it == "line")
 *                                {
 *                                    uint16_t v0, v1;
 *                                    convertTokens(++it, tokens.end(), v0, v1);
 *                                    if (!level.removeLine(v0, v1))
 *                                        message.getText() = "Error: level.removeLine() failed";
 *                                }
 *                                else
 *                                {
 *                                    message.getText() = "Error: delete: Command does not exist";
 *                                }
 *                            }
 *                            else
 *                            {
 *                                message.getText() = "Error: delete: Wrong number of tokens";
 *                            }
 *                        }
 *                        else
 *                        {
 *                            message.getText() = "Error: Command does not exist";
 *                        }
 *                    }
 *                    catch (std::runtime_error &exception)
 *                    {
 *                        message.getText() = exception.what();
 *                    }
 *                }
 *            }
 *
 *            default:
 *                break;
 *            }
 *        }
 *        break;
 *
 *    case SDL_TEXTINPUT:
 *        input.getText() += event.text.text;
 *        break;
 *
 *    default:
 *        break;
 *    }
 *}
 *
 */
