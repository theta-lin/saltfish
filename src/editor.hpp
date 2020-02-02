#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <iostream>
#include "game.hpp"
#include "io.hpp"
#include "line_shape.hpp"
#include "ui.hpp"

/*
 * TODO: REPLACE ALL OF THESE WITH A PROPER EDITOR.
 * TODO: Add in-game preview.
 * TODO: Add the ability to highlight selected objects.
 *       (also need to add more shapes)
 * NOTE: To differentiate between canvas and preview,
 *       use "editor" or "preview" in names.
 *
 * A game level editor, will be hooked onto UI by EditorState of ProgramState.
 * It displays the objects on screen, and forwards most of the events to tool.
 * The design consideration is to have the editor deal with screen, and tool deal with level.
 * WHEN FORWARDING EVENTS: ALL SCREEN COORDINATES ARE TRANSLATED INTO REAL COORDINATES,
 * and A LEFT MOUSE BUTTON UP IS ONLY FORWARDED WHEN IT HAS THE SAME COORDINATES AS THE PREVIOUS LMB DOWN EVENT
 * (otherwise considered a drag operation).
 * Note: Capture BUTTON DOWN only for UPDATING, and LMB UP for the ACTUAL CLICK.
 *
 * Editor also provides the access of level, status, and message to tool;
 * and it receives the operation to be appended to history.
 * Tool also gives some hints to UI (TODO).
 *
 *                      +-------------------------------------------+
 *                      |                                           |
 * +--------+     +-----------+             +---------+             v
 * |  UI    |====>|           |   Events    |         |     +----------------+
 * | Events |     |           |------------>|         |---->| Status String  |
 * +--------+     |  Editor   |UI, Operation|         |     +----------------+
 *                |           |<------------|  Tool   |
 *                |+---------+|  Interface  |         |     +----------------+
 *                || History ||------------>|         |---->| Message String |
 *                |+---------+|             |         |     +----------------+
 *                +-----------+             +---------+
 *                      ^                        ^
 *                      |        +-------+       |
 *                      +--------| Level +-------+
 *                               +-------+
 *
 *
 * Note: It actually store a Game object instead of simply a Level,
 *       although it won't be needed now, it will be useful for the preview function in the future.
 */
class Editor final : public Widget
{
private:
	/*
	 * x, y: origin(left upper corner) coordinates(real)
	 * scale: ? m(real) = 1 px(screen)
	 */
	struct ViewRect
	{
		Vec2d origin;
		double scale;
	};

	/*
	 * The current tool is kept similar to a state machine,
	 * the next tool will be returned through handleEvent().
	 */
	class Tool
	{
	protected:
		// Distance(proportional to view HEIGHT, in REAL distance on CANVAS) from the object that,
		// during mouse click, will still count as selecting the object.
		// Note: Uses view height because view width varies a lot.
		const double vertexCanvasEps{0.002};
		const double lineCanvasEps{0.002};

		Editor &editor;

	public:
		Tool(Editor &editor);
		virtual ~Tool();

		// Received SDL_Event, if containing coordinates, will be translated into real coordinates.
		// Note: Coordinates in "event" is screen coordinates, "coord" is real coordinates
		// Return true to append to history,
		// and a new tool or nullptr for no change.
		virtual std::pair<bool, std::unique_ptr<Tool> > handleEvent(const SDL_Event &event, const Vec2d &coord) = 0;

		// Activate(acting as redo if called from outside) and undo operation, supposed to be called from History.
		// activate() can also be called internally by handleEvent()
		// Throws EXCEPTION if failed (it is supposed to be a valid operation when appended to History).
		virtual void activate() = 0;
		virtual void undo() = 0;
	};

	class NullTool : public Tool
	{
	private:
		const std::string defaultStatus{"NULL TOOL: "};
		const std::string defaultStatusPrompt{"Press \'q\' to quit"};
		const std::string quitPromptStatus{"Are you sure to quit? [y/N]"};
		// true for waiting for the user to confirm quitting
		bool confirmQuit;

	public:
		NullTool(Editor &editor);
		std::pair<bool, std::unique_ptr<Tool> > handleEvent(const SDL_Event &event, const Vec2d &coord) override;
		virtual void activate() override;
		virtual void undo() override;
	};

	/*
	 * Maintains a pointer to a list of operations in order to undo/redo by moving the pointer.
	 * The current pointer points at one past the last operation.
	 */
	class History
	{
	private:
		std::list<std::unique_ptr<Tool> > operations;
		std::list<std::unique_ptr<Tool> >::iterator current;

	public:
		History();
		void append(std::unique_ptr<Tool> tool);

		// Returns true if undo/redo succeeded.
		// NOTE: true means it is able to MOVE THE CURRENT POINTER,
		//       not the OPERATION it tries to undo succeeded.
		//       If the operation itself failed, the exception will be handled and logged.
		bool undo();
		bool redo();
	};

	// Initial view scale
	const double initScale{0.05};

	ViewRect view;

public:
	Log &logger;

private:
	sw::Window &window;

public:
	Game &game;

	// Display current tool and input for tool
	std::string &status;
	// Display input to EDITOR itself, messages, and errors
	std::string &message;

private:
	std::unique_ptr<Tool> tool;
	History history;

	// Track the status of horizontal/vertical locking of mouse
	bool    hLocked;
	int32_t yAlign;
	bool    vLocked;
	int32_t xAlign;

	// Allow dragging the map
	bool leftMouseButtonDown;
	bool dragged;
	int32_t xOrigin;
	int32_t yOrigin;

	// Corresponding real coordinates of mouse
	Vec2d mouseReal;

	// change of view scale / mouse wheel scroll amount
	const double zoomCoeff{0.001};
	// min and max is actually opposite (min = max zoom in)
	const double zoomMin{0.001};
	const double zoomMax{1.0};

	const sw::Color foregroundColor{255, 255, 255, 255};
	const sw::Color backgroundColor{0  ,   0,   0, 255};

	// Wrapper to deal with tool pointer and history after tool handled event
	void toolHandleEvent(const SDL_Event &event, const Vec2d &coord);

public:
	const std::function<void()> onExit;

	Editor(const DoubleRect &dimension, Log &logger, sw::Window &window, Game &game, std::string &status, std::string &message, std::function<void()> onExit);
	void handleEvent(const SDL_Event &event) final;
	void draw(sw::Surface &surface) final;

	// REAL height of view
	double getViewHeight();
};

#endif // ifndef EDITOR_HPP

