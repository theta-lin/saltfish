#include "ui.hpp"

Widget::Widget(const DoubleRect &dimension) : dimension{dimension}
{
}

void Widget::reInit(int wScreen, int hScreen)
{
	real.x = dimension.x * wScreen;
	real.y = dimension.y * hScreen;
	real.w = dimension.w * wScreen;
	real.h = dimension.h * hScreen;
}

void Widget::handleEvent([[maybe_unused]] const SDL_Event &event)
{
}

TextBar::TextBar(const DoubleRect &dimension, const sw::ColorPair &color, const std::filesystem::path &fontPath)
	: Widget{dimension}, fontPath{fontPath}, color{color}
{
}

void TextBar::reInit(int wScreen, int hScreen)
{
	Widget::reInit(wScreen, hScreen);
	if (font)
		font.close();
	font.open(fontPath, static_cast<int>(real.h * fontScale));
}

void TextBar::draw(sw::Surface &surface)
{
	surface.fillRect(&real, color.second);
	if (text.size() > 0)
	{
		sw::Surface textRender(font.renderBlended(text, color.first));
		sw::Rect dstRect{real.x, real.y + static_cast<int>(real.h * (1.0 - fontScale) * 0.5), 0, 0};
		textRender.blit(surface, nullptr, &dstRect);
	}
}

Menu::Item::Item(std::string_view text, std::function<void()> onActivation, bool enable)
	: text{text}, onActivation{onActivation}, enable{enable}
{
}

Menu::Item::Item(const Item &item)
	: text{item.text}, onActivation{item.onActivation}, enable{item.enable}
{
}

Menu::Item& Menu::Item::operator=(const Menu::Item &item)
{
	text = item.text;
	onActivation = item.onActivation;
	enable = item.enable;
	return *this;
}

void Menu::Item::update(int width, int height, const sw::ColorPair &color, sw::Font &font)
{
	if (cache)
		cache.free();
	cache.create(width, height);
	cache.fillRect(nullptr, color.second);

	// The space is for preventing the text from sticking to the left
	sw::Surface textRender{font.renderBlended(' ' + text, color.first)};
	sw::Rect dstRect{0, static_cast<int>(height * (1.0 - fontScale) * 0.5), 0, 0};
	textRender.blit(cache, nullptr, &dstRect);
}

void Menu::Item::activate()
{
	if (enable && onActivation)
		onActivation();
}

void Menu::updateItem(int index)
{
	if (index == noSelected)
		return;

	if (index == selected)
	{
		if (items[index].enable)
		{
			items[index].update(real.w, itemHeightReal, selectedColor, font);
		}
		else
		{
			items[index].update(real.w, itemHeightReal, disabledSelectedColor, font);
		}
	}
	else
	{
		if (items[index].enable)
		{
			items[index].update(real.w, itemHeightReal, normalColor, font);
		}
		else
		{
			items[index].update(real.w, itemHeightReal, disabledNormalColor, font);
		}
	}
}

int Menu::itemUnderCursor(int x, int y)
{
	if (itemHeightReal == 0 || gapHeightReal == 0)
		return noSelected;

	x -= real.x;
	y -= real.y;
	if (   x < 0      || y < 0
		|| x > real.w || y >= static_cast<int>(items.size()) * (itemHeightReal + gapHeightReal))
		return noSelected;

	int index {y / (itemHeightReal + gapHeightReal)};
	int offset{y % (itemHeightReal + gapHeightReal)};
	if (offset <= itemHeightReal)
		return index;
	else
		return noSelected;
}

Menu::Menu(const DoubleRect &dimension, double itemHeight, double gapHeight, const sw::ColorPair &normalColor, const sw::ColorPair &selectedColor, const sw::ColorPair &disabledNormalColor, const sw::ColorPair &disabledSelectedColor, const std::filesystem::path &fontPath)
	: Widget{dimension}, itemHeight{itemHeight}, gapHeight{gapHeight},
	  normalColor{normalColor}, selectedColor{selectedColor},
	  disabledNormalColor{disabledNormalColor}, disabledSelectedColor{disabledSelectedColor},
	  fontPath{fontPath}, selected{noSelected}
{
}

void Menu::reInit(int wScreen, int hScreen)
{
	Widget::reInit(wScreen, hScreen);
	itemHeightReal = static_cast<int>(itemHeight * hScreen);
	gapHeightReal  = static_cast<int>(gapHeight * hScreen);

	if (font) font.close();
	font.open(fontPath, static_cast<int>(itemHeight * hScreen * Item::fontScale));

	for (int i{0}; i < static_cast<int>(items.size()); ++i)
		updateItem(i);
}

void Menu::handleEvent(const SDL_Event &event)
{
	if (items.size() == 0)
		return;

	switch (event.type)
	{
	case SDL_KEYDOWN:
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
				if (static_cast<std::size_t>(selected) == items.size())
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
		break;

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
		break;
	}

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
	for (std::size_t i{0}; i < items.size(); ++i)
	{
		items[i].cache.blit(surface, nullptr, &current);
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

UI::UI(sw::Surface &surface) : surface{&surface}
{
}

void UI::reInit(sw::Surface &surface)
{
	this->surface = &surface;
	for (auto &widget : widgets)
		widget->reInit(surface.getWidth(), surface.getWidth());
}

void UI::update()
{
	for (auto &widget : widgets)
		widget->draw(*surface);
}

void UI::handleEvent(const SDL_Event &event)
{
	for (auto &widget : widgets)
		widget->handleEvent(event);
}

void UI::add(Widget &widget)
{
	widgets.push_back(&widget);
	widget.reInit(surface->getWidth(), surface->getHeight());
}

void UI::remove(Widget &widget)
{
	widgets.remove(&widget);
}

static const sw::ColorPair normalColor{{255, 255, 255, 255}, {80, 80, 80, 240}};
static const sw::ColorPair selectedColor{{255, 130, 0, 255}, {255, 255, 255, 240}};
static const sw::ColorPair disabledNormalColor{{80, 80, 80, 255}, {120, 120, 120, 240}};
static const sw::ColorPair disabledSelectedColor{{120, 120, 120, 255}, {180, 180, 180, 240}};

Menu makeMainMenu(const DoubleRect &dimension, double itemHeight, double gapHeight, const std::filesystem::path &fontDir)
{
	return {dimension, itemHeight, gapHeight,
	        normalColor, selectedColor, disabledNormalColor, disabledSelectedColor,
	        fontDir / "Terminus-Bold.ttf"};
}

