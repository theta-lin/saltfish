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
	virtual void reInit(int wScreen, int hScreen);
	virtual void handleEvent([[maybe_unused]] const sw::Event &event);
	virtual void draw(sw::Surface &surface) = 0;
};

class DrawableWidget : public Widget
{
private:
	std::function<void(sw::Surface&, const sw::Rect&)> onDraw;

public:
	DrawableWidget(const doubleRect &dimension, std::function<void(sw::Surface&, const sw::Rect&)> onDraw);
	void draw(sw::Surface &surface) override;
};

class Menu final : public Widget
{
public:
	class Item
	{
	private:
		std::string text;
		std::function<void()> onActivation;
		bool isEnable;

		sw::Surface cache;

	public:
		static constexpr double fontScale{0.75};

		Item(std::string_view text, std::function<void()> onActivation, bool isEnable = true);
		Item(const Item &item);
		Item& operator=(const Item &item);

		void setEnable(bool isEnable);
		bool getEnable();
		void update(int width, int height, const sw::ColorPair &color, sw::Font &font);
		sw::Surface& getCache();
		void activate();
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

	int itemHeightReal;
	int gapHeightReal;
	sw::Font font;

	void updateItem(int index);
	int itemUnderCursor(int x, int y);

public:
	static constexpr int begin{-1};
	static constexpr int end{-2};

	Menu(const doubleRect &dimension, double itemHeight, double gapHeight, const sw::ColorPair &normalColor, const sw::ColorPair &selectedColor, const sw::ColorPair &disabledNormalColor, const sw::ColorPair &disabledSelectedColor, const std::filesystem::path &fontPath);
	void reInit(int wScreen, int hScreen) override final;
	void handleEvent(const sw::Event &event) override final;
	void draw(sw::Surface &surface) override final;
	void add(const Item &item, int index = -2);
	void remove(int index = -2);
};

class UI
{
private:
	sw::Surface &surface;
	std::list<Widget*> widgets;

public:
	UI(sw::Surface &surface);
	void reInit();
	sw::Surface& getSurface();
	void update();
	void handleEvent(const sw::Event &event);
	void add(Widget &widget);
	void remove(Widget &widget);
};

#endif // ifndef UI_HPP

