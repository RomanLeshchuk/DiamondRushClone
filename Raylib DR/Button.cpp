#include "Button.h"

#include "EventsHandler.h"

Button::Button(const Text& text, Color defaultColor, Color clickedColor) :
	text{ text }, m_defaultColor{ defaultColor }, m_clickedColor{ clickedColor }
{
}

void Button::draw()
{
	if (m_prevClicked)
	{
		text.color = m_clickedColor;
	}
	else
	{
		text.color = m_defaultColor;
	}

	text.draw();
}

bool Button::isClicked(const EventsHandler& eventsHandler)
{
	bool prevClicked = m_prevClicked;
	std::pair<bool, Coords> clickData = eventsHandler.handleTouch();
	bool inButton = coordsInButton(clickData.second);
	m_prevClicked = clickData.first && inButton;

	return prevClicked && !clickData.first && inButton;
}

bool Button::coordsInButton(const Coords& coords)
{
	int xSize = MeasureText(text.text.c_str(), text.fontSize);
	Coords rectPos{ text.coords.x - xSize / 2, text.coords.y - text.fontSize / 2 };
	Coords rectSize{ xSize, text.fontSize };

	return (coords.x >= rectPos.x && coords.y >= rectPos.y && coords.x <= rectPos.x + rectSize.x && coords.y <= rectPos.y + rectSize.y);
}