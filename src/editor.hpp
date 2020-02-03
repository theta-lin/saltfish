#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <iostream>
#include "game.hpp"
#include "io.hpp"
#include "line_shape.hpp"
#include "ui.hpp"

/*
 * TODO: Add more tools.
 * TODO: Add in-game preview.
 * TODO: Add the ability to highlight selected objects.
 *       (also need to add more shapes)
 * NOTE: To differentiate between canvas and preview,
 *       use "editor" or "preview" in names.
 *
 * A game level editor, will be hooked onto UI by EditorState of ProgramState.
 * It displays the objects on screen, and forwards most of the events to tool.
 * The design consideration is to have the editor deal with screen, and tool deal with events and level.
 * WHEN FORWARDING EVENTS: ALL SCREEN COORDINATES ARE TRANSLATED INTO REAL COORDINATES.
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
 *
 * Operations:
 * ESC: Escape to null tool
 * u: Undo
 * r: Redo
 * LSHIFT: Toggle horizontal locking
 * LALT: Toggle vertical locking
 * Drag mouse (hold LMB down): Drag view
 * Mouse wheel: Zooming
 * q: Quit, always ask confirmation
 * n: New, ask confirmation when level changed
 * o: Open, ask confirmation when level changed
 * Move mouse or press mouse button on null tool: Show coordinates
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
	private:
		static constexpr std::string_view undoSucMessage{"Undo one operation"};
		static constexpr std::string_view undoFailMessage{"Already at oldest change"};
		static constexpr std::string_view redoSucMessage{"Redo one operation"};
		static constexpr std::string_view redoFailMessage{"Already at newest change"};

		static constexpr std::string_view hLockedMessage{"Horizontal locked"};
		static constexpr std::string_view hUnlockedMessage{"Horizontal unlocked"};
		static constexpr std::string_view vLockedMessage{"Vertical locked"};
		static constexpr std::string_view vUnlockedMessage{"Vertical unlocked"};

		// Allow dragging the map
		bool leftMouseButtonDown;
		bool dragged;

	protected:
		/*
		 * Helper to capture a single "piece" of argument from events.
		 * A tool can use a series of fields to capture its arguments.
		 */
		class Field
		{
		public:
			enum FieldNext
			{
				unhandled, // Event not handled
				keep,      // Stay at current field
				previous,  // Go back a field (or, if at beginning, quit)
				next       // Advance a field (or, if captured all arguments, begin processing)
			};

			virtual ~Field();
			virtual FieldNext handleEvent(const SDL_Event &event) = 0;
		};

		class TextField : public Field
		{
		private:
			std::string text;

		public:
			TextField();
			~TextField() override;
			FieldNext handleEvent(const SDL_Event &event) override;
			const std::string& getText();
		};

		// Distance(proportional to view HEIGHT, in REAL distance on CANVAS) from the object that,
		// during mouse click, will still count as selecting the object.
		// Note: Uses view height because view width varies a lot.
		const double vertexCanvasEps{0.002};
		const double lineCanvasEps{0.002};

		Editor &editor;

	public:
		Tool(Editor &editor);
		virtual ~Tool();

		// Receives SDL_Event, in which if containing coordinates, needs to be translated into real coordinates.
		// It reads the real coordinates through Editor::getMouseReal(), and rarely access the screen coordinates in SDL_Event
		//
		// Return:
		// bool done: true if event has been FINALLY handled, and SHOULDN'T be handled again
		//            (doesn't really matter when this tool doesn't overshadow other tools)
		// bool appendTo: true to append to history,
		// std::unique_ptr<Tool> next: the next tool or nullptr for no change.
		//
		// This function in base class implements a "basic" event handle.
		// It handles history navigation, mouse horizontal/vertical locking, canvas dragging, and zooming.
		// The bookkeeping of mouse position and warping mouse is delegated to editor.
		// A LEFT MOUSE BUTTON UP IS ONLY UNHANDLED WHEN IT HAS THE SAME COORDINATES AS THE PREVIOUS LMB DOWN EVENT
		// (otherwise considered a dragging operation).
		//  Note: Derivatives should handle LEFT MOUSE BUTTON DOWN only for UPDATING, and LEFT MOUSE BUTTON UP for the ACTUAL CLICK.
		virtual std::tuple<bool, bool, std::unique_ptr<Tool> > handleEvent(const SDL_Event &event);

		// Activate(acting as redo if called from outside) and undo operation, supposed to be called from History.
		// activate() can also be called internally by handleEvent()
		// Throws EXCEPTION if failed (it is supposed to be a valid operation when appended to History).
		virtual void activate();
		virtual void undo();
	};

	/*
	 * The "tool" of not having any tool
	 */
	class NullTool final : public Tool
	{
	private:
		static constexpr std::string_view defaultStatus{"NULL: "};
		static constexpr std::string_view quitPromptStatus{"Are you sure to quit? [y/N]"};
		static constexpr std::string_view newPromptStatus{"Are you sure to begin a new level? [y/N]"};
		static constexpr std::string_view newMessage{"New level"};

		// true for awaiting user's confirmation
		bool confirmQuit;
		bool confirmNew;

		void showDefaultStatus();
		void newLevel();

	public:
		NullTool(Editor &editor);
		std::tuple<bool, bool, std::unique_ptr<Tool> > handleEvent(const SDL_Event &event) override;
	};

	class OpenTool final: public Tool
	{
	private:
		static constexpr std::string_view defaultStatusPrompt{"Open: "};
		static constexpr std::string_view cursorChar{"|"};
		static constexpr std::string_view OpenPromptStatus0{"Are you sure to open: "};
		static constexpr std::string_view OpenPromptStatus1{"?"};

		TextField textField;
		bool confirmOpen;

		void openLevel();

	public:
		OpenTool(Editor &editor);
		std::tuple<bool, bool, std::unique_ptr<Tool> > handleEvent(const SDL_Event &event) override;
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
		void clear();
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
	int32_t xOrigin;
	int32_t yOrigin;

	// Corresponding real coordinates of mouse
	Vec2d mouseReal;

	// change of view scale / mouse wheel scroll amount
	const double zoomCoeff{0.1};
	// min and max is actually opposite (min = max zoom in)
	const double zoomMin{0.001};
	const double zoomMax{1.0};

	const sw::Color foregroundColor{255, 255, 255, 255};
	const sw::Color backgroundColor{0  ,   0,   0, 255};

	// Wrapper to deal with tool pointer and history after tool handled event
	void toolHandleEvent(const SDL_Event &event);

public:
	// true for change since last new/load/save
	bool changed;
	const std::function<void()> onExit;

	Editor(const DoubleRect &dimension, Log &logger, sw::Window &window, Game &game, std::string &status, std::string &message, std::function<void()> onExit);
	void handleEvent(const SDL_Event &event) final;
	void draw(sw::Surface &surface) final;

	const Vec2d& getMouseReal();

	bool undo();
	bool redo();
	void clearHistory();

	bool toggleHLock();
	bool toggleVLock();
	void dragTo(int32_t x, int32_t y);
	double zoomView(int32_t zoomInput);
};

#endif // ifndef EDITOR_HPP

