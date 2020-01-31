/*
 * This file contains basic widgets for user interface,
 * and default style of the widgets.
 */

#ifndef UI_HPP
#define UI_HPP

#include "font.hpp"
#include "window.hpp"

/*
 * x, y: origin(left upper corner) coordinates
 * w, h: width, height
 */
struct DoubleRect
{
	double x;
	double y;
	double w;
	double h;
};

class Widget
{
protected:
	// The REAL dimension ON SCREEN (in px)
	sw::Rect real;

public:
	// Dimension proportional to each axis of screen
	const DoubleRect dimension;

	Widget(const DoubleRect &dimension);

	// This method is exposed to allow a Widget to be reinitialized in events such as resizing
	virtual void reInit(int wScreen, int hScreen);

	// NOTE: A widget may have no associated event handler
	virtual void handleEvent([[maybe_unused]] const SDL_Event &event);

	// NOTE: It's up to the IMPLEMENTER to USE real FOR CLIPPING
	virtual void draw(sw::Surface &surface) = 0;
};

/*
 * A bar to display ONE line of text
 * Can be used for displaying entered command like command mode in Vim
 */
class TextBar : public Widget
{
private:
	std::filesystem::path fontPath;
	sw::Font font;
	sw::ColorPair color;

public:
	static constexpr double fontScale{0.75}; // fontHeight / itemHeight

	std::string text;

	TextBar(const DoubleRect &dimension, const sw::ColorPair &color, const std::filesystem::path &fontPath);
	void reInit(int wScreen, int hScreen) override;
	void draw(sw::Surface &surface) override;
};

/*
 * A simple menu with a list of Items like buttons,
 * can be navigated with mouse and arrow keys.
 */
class Menu final : public Widget
{
public:
	class Item
	{
	private:
		std::string text;
		std::function<void()> onActivation;

	public:
		static constexpr double fontScale{0.75}; // fontHeight / itemHeight

		// Prevent rendering multiple times
		// Only updated when the "selected" status change
		sw::Surface cache;

		bool enable;


		Item(std::string_view text, std::function<void()> onActivation, bool enable = true);
		Item(const Item &item);
		Item& operator=(const Item &item);

		void update(int width, int height, const sw::ColorPair &color, sw::Font &font);
		void activate(); // An Item can activated by the parent Menu
	};

private:
	double itemHeight;
	double gapHeight;

	sw::ColorPair normalColor;
	sw::ColorPair selectedColor;
	sw::ColorPair disabledNormalColor;
	sw::ColorPair disabledSelectedColor;

	std::filesystem::path fontPath;
	std::vector<Item> items;
	int selected;
	static constexpr int noSelected{-1};

	int itemHeightReal; // Height of each Item
	int gapHeightReal; // Gap between Items
	sw::Font font;

	void updateItem(int index);
	int itemUnderCursor(int x, int y);

public:
	enum SpecialPosition
	{
		begin = -1,
		end = -2
	};

	Menu(const DoubleRect &dimension, double itemHeight, double gapHeight, const sw::ColorPair &normalColor, const sw::ColorPair &selectedColor, const sw::ColorPair &disabledNormalColor, const sw::ColorPair &disabledSelectedColor, const std::filesystem::path &fontPath);
	void reInit(int wScreen, int hScreen) override;
	void handleEvent(const SDL_Event &event) override;
	void draw(sw::Surface &surface) override;
	void add(const Item &item, int index = end);
	void remove(int index = end);
};

/*
 * Simple UI engine,
 * forwards event and update each widget.
 */
class UI
{
private:
	sw::Surface *surface;
	std::list<Widget*> widgets;

public:
	UI(sw::Surface &surface);
	void reInit(sw::Surface &surface);
	void update();
	void handleEvent(const SDL_Event &event);
	void add(Widget &widget);
	void remove(Widget &widget);
};

/*
 * Helper to construct a menu in default style
 * The style can be changed at the near-bottom part of ui.cpp.
 */

/*
 * MainMenu is the menu displayed after the game started, and after pausing the game.
 */
Menu makeMainMenu(const DoubleRect &dimension, double itemHeight, double gapHeight, const std::filesystem::path &fontDir);

#endif // ifndef UI_HPP

