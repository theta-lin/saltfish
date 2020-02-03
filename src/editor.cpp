#include "editor.hpp"
#include <iostream>

Editor::Tool::Field::~Field()
{
}

Editor::Tool::TextField::TextField()
{
	SDL_StartTextInput();
}

Editor::Tool::TextField::~TextField()
{
	SDL_StopTextInput();
}

Editor::Tool::Field::FieldNext Editor::Tool::TextField::handleEvent(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_BACKSPACE:
			if (!text.empty())
				text.pop_back();
			return keep;

		case SDLK_ESCAPE:
			return previous;

		case SDLK_RETURN:
			return next;

		default:
			break;
		}
		break;

	case SDL_TEXTINPUT:
			text += event.text.text;
		return keep;
	}

	return unhandled;
}

const std::string& Editor::Tool::TextField::getText()
{
	return text;
}

Editor::Tool::Tool(Editor &editor) : leftMouseButtonDown{false}, dragged{false}, editor{editor}
{
}

Editor::Tool::~Tool()
{
}

void Editor::Tool::activate()
{
}

void Editor::Tool::undo()
{
}

std::tuple<bool, bool, std::unique_ptr<Editor::Tool> > Editor::Tool::handleEvent(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			return {true, false, std::make_unique<NullTool>(editor)};

		case SDLK_u:
			if (editor.undo())
				editor.message = undoSucMessage;
			else
				editor.message = undoFailMessage;
			return {true, false, std::make_unique<NullTool>(editor)};

		case SDLK_r:
			if (editor.history.redo())
				editor.message = redoSucMessage;
			else
				editor.message = redoFailMessage;
			return {true, false, std::make_unique<NullTool>(editor)};

		case SDLK_LSHIFT:
			if (editor.toggleHLock())
				editor.message = hLockedMessage;
			else
				editor.message = hUnlockedMessage;
			return {true, false, std::make_unique<NullTool>(editor)};

		case SDLK_LALT:
			if (editor.toggleVLock())
				editor.message = vLockedMessage;
			else
				editor.message = vUnlockedMessage;
			return {true, false, std::make_unique<NullTool>(editor)};

		default:
			break;
		}
		break;

	case SDL_MOUSEMOTION:
		if (leftMouseButtonDown)
		{
			dragged = true;
			editor.dragTo(event.motion.x, event.motion.y);
			editor.message = "Dragging";
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT)
			leftMouseButtonDown = true;
		break;

	case SDL_MOUSEBUTTONUP:
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			leftMouseButtonDown = false;
			bool tmp{dragged};
			dragged = false;
			if (tmp)
				return {true, false, nullptr};
		}
		break;

	case SDL_MOUSEWHEEL:
	{
		int32_t zoomInput{event.wheel.y};
		if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			zoomInput = -zoomInput;
		editor.message = "Zoom 1:" + std::to_string(editor.zoomView(zoomInput));
		return {true, false, nullptr};
	}

	default:
		break;
	}

	return {false, false, nullptr};
}

void Editor::NullTool::showDefaultStatus()
{
	const Vec2d &coord{editor.getMouseReal()};
	editor.status = defaultStatus;
	editor.status += '(' + std::to_string(coord[0]) + ',' + std::to_string(coord[1]) + ')';
}

void Editor::NullTool::newLevel()
{
	editor.game.level.clear();
	editor.changed = false;
	confirmNew = false;
	showDefaultStatus();
	editor.message = newMessage;
}

Editor::NullTool::NullTool(Editor &editor) : Tool{editor}, confirmQuit{false}, confirmNew{false}
{
	showDefaultStatus();
}

std::tuple<bool, bool, std::unique_ptr<Editor::Tool> > Editor::NullTool::handleEvent(const SDL_Event &event)
{
	bool done;
	bool appendTo;
	std::unique_ptr<Tool> next;
	std::tie(done, appendTo, next) = Tool::handleEvent(event);
	if (done)
		return {done, appendTo, std::move(next)};

	switch (event.type)
	{
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_q:
			if (!confirmQuit && !confirmNew)
			{
				confirmQuit = true;
				editor.status = quitPromptStatus;
			}
			break;

		case SDLK_y:
			if (confirmQuit)
				editor.onExit();
			if (confirmNew)
				newLevel();
			break;

		case SDLK_n:
			if (confirmQuit)
			{
				confirmQuit = false;
				showDefaultStatus();
			}
			else if (confirmNew)
			{
				confirmNew = false;
				showDefaultStatus();
			}
			else
			{
				if (editor.changed)
				{
					confirmNew = true;
					editor.status = newPromptStatus;
				}
				else
				{
					newLevel();
				}
			}
			break;

		case 'o':
			return {true, false, std::make_unique<OpenTool>(editor)};

		default:
			break;
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		[[fallthrough]];
	case SDL_MOUSEMOTION:
		if (!confirmQuit && !confirmNew)
			showDefaultStatus();
		break;

	default:
		break;
	}

	return {false, false, nullptr};
}

void Editor::OpenTool::openLevel()
{
	if (!editor.game.level.load(textField.getText()))
		editor.message = "Error: level.load() failed";
}

Editor::OpenTool::OpenTool(Editor &editor) : Tool{editor}, confirmOpen{false}
{
	editor.status = defaultStatusPrompt;
	editor.status += cursorChar;
}

std::tuple<bool, bool, std::unique_ptr<Editor::Tool> > Editor::OpenTool::handleEvent(const SDL_Event &event)
{
	static bool firstTime{true};

	if (confirmOpen)
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_y:
				openLevel();
				return {true, false, std::make_unique<NullTool>(editor)};

			case SDLK_n:
				return {true, false, std::make_unique<NullTool>(editor)};

			default:
				break;
			}
			break;

		default:
			break;
		}

		return Tool::handleEvent(event);
	}
	else
	{
		// Discard the text event generated by the keypress to change tool
		if (firstTime && event.type == SDL_TEXTINPUT)
		{
			firstTime = false;
			return {false, false, nullptr};
		}

		switch (textField.handleEvent(event))
		{
		case Field::unhandled:
			break;

		case Field::keep:
			editor.status = defaultStatusPrompt;
			editor.status += textField.getText();
			editor.status += cursorChar;
			break;

		case Field::previous:
			return Tool::handleEvent(event);

		case Field::next:
			if (editor.changed)
			{
				confirmOpen = true;
				break;
			}
			else
			{
				openLevel();
				return {true, false, std::make_unique<NullTool>(editor)};
			}
		}
	}

	return {false, false, nullptr};
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

void Editor::History::clear()
{
	operations.clear();
}

void Editor::toolHandleEvent(const SDL_Event &event)
{
	bool done;
	bool appendTo;
	std::unique_ptr<Tool> next;
	std::tie(done, appendTo, next) = tool->handleEvent(event);

	if (next)
	{
		if (appendTo)
		{
			history.append(std::move(tool));
		}
		tool = std::move(next);
	}
}

Editor::Editor(const DoubleRect &dimension, Log &logger, sw::Window &window, Game &game, std::string &status, std::string &message, std::function<void()> onExit)
	: Widget{dimension}, view{{0.0, 0.0}, initScale},
	  logger{logger}, window{window}, game{game}, status{status}, message{message},
	  tool{std::make_unique<NullTool>(*this)},
	  hLocked{false}, yAlign{-1}, vLocked{false}, xAlign{-1},
	  xOrigin{-1}, yOrigin{-1},
	  changed{false}, onExit{onExit}
{
}

void Editor::handleEvent(const SDL_Event &event)
{
	switch (event.type)
	{
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

		mouseReal = {static_cast<double>(event.motion.x), static_cast<double>(event.motion.y)};
		mouseReal *= view.scale;
		mouseReal += view.origin;
		toolHandleEvent(event);

		xOrigin = event.motion.x;
		yOrigin = event.motion.y;
		break;
	}

	case SDL_MOUSEBUTTONDOWN:
		[[fallthrough]];
	case SDL_MOUSEBUTTONUP:
	{
		// Useful for handling the FIRST left mouse button down IMMEDIATELY after editor start
		xOrigin = event.button.x;
		yOrigin = event.button.y;

		mouseReal = {static_cast<double>(event.button.x), static_cast<double>(event.button.y)};
		mouseReal *= view.scale;
		mouseReal += view.origin;
		toolHandleEvent(event);

		break;
	}

	default:
		toolHandleEvent(event);
		break;
	}
}

void Editor::draw(sw::Surface &surface)
{
	surface.fillRect(&real, backgroundColor);
	sw::Surface tmp{real.w, real.h};
	auto vertices{game.level.getVertices()};
	for (const Level::Line &line : game.level.getLines())
	{
		LineShape lineDraw{
		                  	(vertices[line.v0] - view.origin) / view.scale,
							(vertices[line.v1] - view.origin) / view.scale
		                  };
		lineDraw.draw(foregroundColor, tmp);
	}
	tmp.blit(surface, nullptr, &real);
}

const Vec2d& Editor::getMouseReal()
{
	return mouseReal;
}

bool Editor::undo()
{
	return history.undo();
}

bool Editor::redo()
{
	return history.redo();
}

void Editor::clearHistory()
{
	history.clear();
}

bool Editor::toggleHLock()
{
	hLocked = !hLocked;
	vLocked = false;
	return hLocked;
}

bool Editor::toggleVLock()
{
	vLocked = !vLocked;
	hLocked = false;
	return vLocked;
}

void Editor::dragTo(int32_t x, int32_t y)
{
	// Here we are dragging the view OPPOSITE of mouse's motion,
	// because we actually want to drag the CANVAS not the VIEW
	view.origin[0] += (xOrigin - x) * view.scale;
	view.origin[1] += (yOrigin - y) * view.scale;
}

double Editor::zoomView(int32_t zoomInput)
{
	// Positive wheel.y is scrolling mouse wheel away from user,
	// so making it negative to have it zoom IN.
	// Multiply by current scale to make zooming easier.
	double zoom{-zoomCoeff * zoomInput * view.scale};
	zoom = std::clamp(view.scale + zoom, zoomMin, zoomMax) - view.scale;
	view.origin += (view.origin - mouseReal) / view.scale * zoom;
	return view.scale += zoom;
}

