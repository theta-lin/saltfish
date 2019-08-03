#include "ui.hpp"

Widget::Widget(const doubleRect &dimension) : dimension{dimension}
{
}

void Widget::reInit(int wScreen, int hScreen)
{
	real.x = dimension.x * wScreen;
	real.y = dimension.y * hScreen;
	real.w = dimension.w * wScreen;
	real.h = dimension.h * hScreen;
}

doubleRect Widget::getDimension()
{
	return dimension;
}

void Widget::handleEvent([[maybe_unused]] const sw::Event &event)
{
}

DrawableWidget::DrawableWidget(const doubleRect &dimension, std::function<void(sw::Surface&, const sw::Rect&)> onDraw)
	: Widget{dimension}, onDraw{onDraw}
{
}

void DrawableWidget::draw(sw::Surface &surface)
{
	onDraw(surface, real);
}

Menu::Item::Item(std::string_view text, std::function<void()> onActivation, bool isEnable)
	: text{text}, onActivation{onActivation}, isEnable{isEnable}
{
}

Menu::Item::Item(const Item &item)
	: text{item.text}, onActivation{item.onActivation}, isEnable{item.isEnable}
{
}

Menu::Item& Menu::Item::operator=(const Menu::Item &item)
{
	text = item.text;
	onActivation = item.onActivation;
	isEnable = item.isEnable;
	return *this;
}

void Menu::Item::setEnable(bool isEnable)
{
	this->isEnable = isEnable;
}

bool Menu::Item::getEnable()
{
	return isEnable;
}

void Menu::Item::update(int width, int height, const sw::ColorPair &color, sw::Font &font)
{
	if (cache)
		cache.free();
	cache.create(width, height);
	cache.fillRect(nullptr, color.second);
	sw::Surface textRender{font.renderBlended(' ' + text, color.first)};
	textRender.blit(cache, nullptr, nullptr);
}

sw::Surface& Menu::Item::getCache()
{
	return cache;
}

void Menu::Item::activate()
{
	if (isEnable && onActivation)
		onActivation();
}

void Menu::updateItem(int index)
{
	if (index == noSelected)
		return;

	if (index == selected)
		items[index].update(real.w, itemHeightReal, selectedColor, font);
	else if (items[index].getEnable())
		items[index].update(real.w, itemHeightReal, normalColor, font);
	else
		items[index].update(real.w, itemHeightReal, disabledColor, font);
}

int Menu::itemUnderCursor(int x, int y)
{
	if (itemHeightReal == 0 || gapHeightReal == 0)
		return noSelected;

	x -= real.x;
	y -= real.y;
	if (   x < 0 || y < 0
		|| x > real.w || y >= static_cast<int>(items.size()) * (itemHeightReal + gapHeightReal))
		return noSelected;

	int index{y / (itemHeightReal + gapHeightReal)};
	int offset{y % (itemHeightReal + gapHeightReal)};
	if (offset <= itemHeightReal)
		return index;
	else
		return noSelected;
}

Menu::Menu(const doubleRect &dimension, double itemHeight, double gapHeight, const sw::ColorPair &normalColor, const sw::ColorPair &disabledColor, const sw::ColorPair &selectedColor, const std::filesystem::path &fontPath)
	: Widget{dimension}, itemHeight{itemHeight}, gapHeight{gapHeight}, normalColor{normalColor}, disabledColor{disabledColor}, selectedColor{selectedColor}, fontPath{fontPath}
{
	selected = noSelected;
}

void Menu::reInit(int wScreen, int hScreen)
{
	Widget::reInit(wScreen, hScreen);
	itemHeightReal = static_cast<int>(itemHeight * hScreen);
	gapHeightReal = static_cast<int>(gapHeight * hScreen);

	if (font)
		font.close();

	static const double fontScale{0.8};
	font.open(fontPath, static_cast<int>(itemHeight * hScreen * fontScale));

	for (int i{0}; i < static_cast<int>(items.size()); ++i)
		updateItem(i);
}

void Menu::handleEvent(const sw::Event &event)
{
	if (items.size() == 0)
		return;

	switch (event.type)
	{
	case SDL_KEYDOWN:
		{
			switch (event.key.keysym.scancode)
			{
			case SDL_SCANCODE_DOWN:
				if (selected == noSelected)
				{
					selected = 0;
					updateItem(selected);
				}
				else
				{
					int origin{selected};
					++selected;
					if (static_cast<size_t>(selected) == items.size())
						selected = 0;
					updateItem(origin);
					updateItem(selected);
				}
				break;

			case SDL_SCANCODE_UP:
				if (selected == noSelected)
				{
					selected = 0;
					updateItem(selected);
				}
				else
				{
					int origin{selected};
					--selected;
					if (selected == -1)
						selected = static_cast<int>(items.size() - 1);
					updateItem(origin);
					updateItem(selected);
				}
				break;

			case SDL_SCANCODE_RETURN:
				if (selected == noSelected)
				{
					selected = 0;
					updateItem(selected);
				}
				else
				{
					items[selected].activate();
				}
				break;

			default:
				break;
			}
		}

	case SDL_MOUSEMOTION:
		{
			int index{itemUnderCursor(event.motion.x, event.motion.y)};
			if (index != noSelected)
			{
				int origin{selected};
				selected = index;
				updateItem(origin);
				updateItem(selected);
			}
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (   event.button.button == SDL_BUTTON_LEFT
			&& selected != noSelected
			&& itemUnderCursor(event.button.x, event.button.y) == selected)
			items[selected].activate();
		break;

	default:
		break;
	}
}

void Menu::draw(sw::Surface &surface)
{
	sw::Rect current{real.x, real.y, 0, 0};
	for (size_t i{0}; i < items.size(); ++i)
	{
		items[i].getCache().blit(surface, nullptr, &current);
		current.y += itemHeightReal + gapHeightReal;
	}
}

void Menu::add(const Item &item, int index)
{
	static const double epsilon{0.01};
	if ((items.size() + 1) * itemHeight - dimension.h > epsilon)
		throw std::runtime_error("Menu::add() failed: total item height exceeding menu height");

	if (index == begin)
		items.insert(items.begin(), item);
	else if (index == end)
		items.push_back(item);
	else
		items.insert(items.begin() + index, item);
}

void Menu::remove(int index)
{
	if (index == begin)
	{
		items.erase(items.begin());
	}
	else if(index == end)
	{
		items.pop_back();
	}
	else
	{
		if (index == selected)
			selected = noSelected;
		items.erase(items.begin() + index);
	}
}

UI::UI(sw::Surface &surface) : surface{surface}
{
}

void UI::reInit()
{
	for (auto &widget : widgets)
		widget->reInit(surface.getWidth(), surface.getWidth());
}

void UI::update()
{
	for (auto &widget : widgets)
		widget->draw(surface);
}

void UI::handleEvent(const sw::Event &event)
{
	for (auto &widget : widgets)
		widget->handleEvent(event);
}

void UI::add(Widget &widget)
{
	widgets.push_back(&widget);
	widget.reInit(surface.getWidth(), surface.getHeight());
}

void UI::remove(Widget &widget)
{
	widgets.remove(&widget);
}

