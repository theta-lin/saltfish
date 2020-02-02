#include "editor.hpp"
#include <iostream>

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
				editor.status = defaultStatus + defaultStatusPrompt;
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

Editor::Editor(const DoubleRect &dimension, Log &logger, sw::Window &window, Game &game, std::string &status, std::string &message, std::function<void()> onExit)
	: Widget{dimension}, view{{0.0, 0.0}, initScale},
	  logger{logger}, window{window}, game{game}, status{status}, message{message},
	  tool{std::make_unique<NullTool>(*this)},
	  hLocked{false}, yAlign{-1}, vLocked{false}, xAlign{-1},
	  leftMouseButtonDown{false}, dragged{false}, xOrigin{-1}, yOrigin{-1},
	  onExit{onExit}
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
		// Handling the FIRST mouse motion IMMEDIATELY after editor start
		if (xAlign == -1) xAlign = event.motion.x;
		if (yAlign == -1) yAlign = event.motion.y;

		// Note: Warping mouse generates another mouse motion event (which must be valid)
		if (hLocked && event.motion.y != yAlign)
		{
			SDL_WarpMouseInWindow(window.getPtr(), event.motion.x, yAlign);
			break;
		}
		if (vLocked && event.motion.x != xAlign)
		{
			SDL_WarpMouseInWindow(window.getPtr(), xAlign, event.motion.y);
			break;
		}

		xAlign = event.motion.x;
		yAlign = event.motion.y;

		if (leftMouseButtonDown)
		{
			dragged = true;

			// Here we are dragging the view OPPOSITE of mouse's motion,
			// because we actually want to drag the CANVAS not the VIEW
			view.origin[0] += (xOrigin - event.motion.x) * view.scale;
			view.origin[1] += (yOrigin - event.motion.y) * view.scale;
			message = "Dragging";
		}

		mouseReal = {static_cast<double>(event.motion.x), static_cast<double>(event.motion.y)};
		mouseReal *= view.scale;
		mouseReal += view.origin;
		toolHandleEvent(event, mouseReal);

		xOrigin = event.motion.x;
		yOrigin = event.motion.y;
		break;
	}

	case SDL_MOUSEBUTTONDOWN:
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			// Actually, only useful for handling the FIRST left mouse button down IMMEDIATELY after editor start
			xOrigin = event.button.x;
			yOrigin = event.button.y;

			leftMouseButtonDown = true;
		}

		mouseReal = {static_cast<double>(event.button.x), static_cast<double>(event.button.y)};
		mouseReal *= view.scale;
		mouseReal += view.origin;
		toolHandleEvent(event, mouseReal);

		break;
	}

	case SDL_MOUSEBUTTONUP:
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			if (!dragged)
			{
				mouseReal = {static_cast<double>(event.button.x), static_cast<double>(event.button.y)};
				mouseReal *= view.scale;
				mouseReal += view.origin;
				toolHandleEvent(event, mouseReal);
			}
			leftMouseButtonDown = false;
			dragged = false;
		}
		break;

	case SDL_MOUSEWHEEL:
	{
		// Positive wheel.y is scrolling mouse wheel away from user,
		// so making it negative to have it zoom IN
		double zoom{-zoomCoeff * event.wheel.y};
		if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			zoom = -zoom;
		zoom = std::clamp(view.scale + zoom, zoomMin, zoomMax) - view.scale;
		view.origin += (view.origin - mouseReal) / view.scale * zoom;
		view.scale += zoom;
		message = "Zoom 1:" + std::to_string(view.scale);

		break;
	}

	default:
		break;
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

double Editor::getViewHeight()
{
	return real.h / view.scale;
}

