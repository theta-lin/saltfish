/*
 * This file contains basic widgets for user interface
 */

#ifndef UI_HPP
#define UI_HPP

#include <cstddef>
#include <string>
#include <string_view>
#include <list>
#include <utility>
#include <stdexcept>
#include <SDL.h>
#include "log.hpp"
#include "config.hpp"
#include "surface.hpp"
#include "font.hpp"
#include "window.hpp"
#include "event.hpp"

struct doubleRect
{
	double x;
	double y;
	double w;
	double h;
};

class Widget
{
protected:
	doubleRect dimension;
	sw::Rect real;
	Widget(const doubleRect &dimension);

public:
	doubleRect getDimension();

	// This method is exposed to allow a Widget to be reinitialized in situations such as resize
	virtual void reInit(int wScreen, int hScreen);

	// NOTE: A widget may have no associated event handler
	virtual void handleEvent([[maybe_unused]] const sw::Event &event);
	virtual void draw(sw::Surface &surface) = 0;
};

/*
 * Implementing a simple canvas to draw anything
 */
class DrawableWidget : public Widget
{
private:
	std::function<void(sw::Surface&)> onDraw;

public:
	DrawableWidget(const doubleRect &dimension, std::function<void(sw::Surface&)> onDraw);
	void draw(sw::Surface &surface) override;
};

/*
 * A bar to display one line of text
 * Can be used for displaying entered command like command mode in Vim
 */
class TextBar : public Widget
{
private:
	std::string text;
	std::filesystem::path fontPath;
	sw::Font font;
	sw::ColorPair color;

public:
	static constexpr double fontScale{0.75};

	TextBar(const doubleRect &dimension, const sw::ColorPair &color, std::filesystem::path fontPath);
	void reInit(int wScreen, int hScreen) override;
	void draw(sw::Surface &surface) override;
	std::string& getText();
};

/*
 * A simple menu with a list of Items like buttons
 */
class Menu final : public Widget
{
public:
	class Item
	{
	private:
		std::string text;
		std::function<void()> onActivation;
		bool isEnable;

		// Prevent rendering multiple times
		sw::Surface cache;

	public:
		static constexpr double fontScale{0.75}; // fontHeight / itemHeight

		Item(std::string_view text, std::function<void()> onActivation, bool isEnable = true);
		Item(const Item &item);
		Item& operator=(const Item &item);

		void setEnable(bool isEnable);
		bool getEnable();
		void update(int width, int height, const sw::ColorPair &color, sw::Font &font);
		sw::Surface& getCache();
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
	static constexpr int begin{-1};
	static constexpr int end{-2};

	Menu(const doubleRect &dimension, double itemHeight, double gapHeight, const sw::ColorPair &normalColor, const sw::ColorPair &selectedColor, const sw::ColorPair &disabledNormalColor, const sw::ColorPair &disabledSelectedColor, const std::filesystem::path &fontPath);
	void reInit(int wScreen, int hScreen) override;
	void handleEvent(const sw::Event &event) override;
	void draw(sw::Surface &surface) override;
	void add(const Item &item, int index = end);
	void remove(int index = end);
};

class UI
{
private:
	sw::Surface *surface;
	std::list<Widget*> widgets;

public:
	UI(sw::Surface &surface);
	void reInit(sw::Surface &surface);
	sw::Surface& getSurface();
	void update();
	void handleEvent(const sw::Event &event);
	void add(Widget &widget);
	void remove(Widget &widget);
};

#endif // ifndef UI_HPP

