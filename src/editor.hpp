#ifndef EDITOR_HPP
#define EDITOR_HPP

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
 * A game level editor, will be hooked on UI by EditorState of ProgramState.
 * It displays the objects on screen, and forwards all the events to Tool,
 * ALL SCREEN COORDINATES ARE TRANSLATED INTO REAL COORDINATES before doing so.
 *
 * +--------+     +-----------+             +---------+
 * |  UI    |====>|  Editor   | EditorEvent |+-------+|
 * | Events |     |+---------+|------------->| Field ||     +----------------+
 * +--------+     || History ||      UI     |+-------+|---->| Status String  |
 *                |+---------+|<------------|         |     +----------------+
 *                |+---------+|             |         |
 *                ||  Level  |------------->|  Tool   |     +----------------+
 *                |+---------+|             |         |---->| Message String |
 *                +-----------+             +---------+     +----------------+
 *
 * Note: It actually store a Game object instead of simply a Level,
 *       although it won't be needed now, it will be useful for the preview function in the future.
 */
class Editor: public Widget
{
private:
	/*
	 * x, y: origin(left upper corner) coordinates(real)
	 * scale: ? m(real) = 1 px(screen)
	 */
	struct ViewRect
	{
		double x;
		double y;
		double scale;
	};

	/*
	 * The current Tool is kept similar to a state machine,
	 * the next Tool will be returned through handleEvent().
	 */
	class Tool
	{
	private:
		// Distance(proportion to screen HEIGHT) from the object that,
		// during mouse click, will still count as selecting the object.
		// Note: Uses screen height because screen width varies a lot.
		const double vertexCanvasEps{0.002};
		const double lineCanvasEps{0.002};

		std::string &status;
		std::string &message;

	public:
		Tool(std::string &status, std::string &message);
		virtual ~Tool();

		// Received SDL_Event, if containing coordinates, will be translated into real coordinates.
		// Return true to append to history,
		// and a new Tool or nullptr for no change.
		virtual std::pair<bool, std::unique_ptr<Tool> > handleEvent(Editor &editor, const SDL_Event &event) = 0;

		// Undo operation, supposed to be called from History.
		// Throws EXCEPTION if failed (it is supposed to be a valid operation when appended to History).
		virtual void undo() = 0;
	};

	class NullTool : public Tool
	{
	public:
		using Tool::Tool;
		std::pair<bool, std::unique_ptr<Tool> > handleEvent(Editor &editor, const SDL_Event &event) override;
		virtual void undo() override;
	};

	/*
	 * Maintains a pointer to a list of operations in order to undo/redo by moving the pointer
	 */
	class History
	{
	private:
		std::list<std::unique_ptr<Tool> > operations;
		std::size_t current;

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

	ViewRect view;
	Game &game;
	std::unique_ptr<Tool> tool;
	std::filesystem::path fontPath;
	sw::Font font;

	// Track the status of horizontal/vertical locking of mouse
	int32_t xOrigin;
	bool    hLocked;
	int32_t yOrigin;
	bool    vLocked;

	// Allow dragging the map
	bool leftMouseButtonDown;

	std::function<void()> onExit;

	void cachedDrawNumber(sw::Surface &surface, const sw::Rect &rect, int number);

public:
	Editor(const DoubleRect &dimension, Game &game, std::string &status, std::string &message, const std::filesystem::path &fontPath, std::function<void()> onExit);
	void handleEvent(const SDL_Event &event);
	void draw(sw::Surface &surface) final;
};

#endif // ifndef EDITOR_HPP

