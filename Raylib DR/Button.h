#pragma once

#include "Text.h"

class EventsHandler;

class Button
{
public:
	Button(const Text& text, Color defaultColor, Color clckedColor);

	void draw();
	bool isClicked(const EventsHandler& eventsHandler);

	Text text;

private:
	bool coordsInButton(const Coords& coords);

	bool m_prevClicked = false;
	Color m_defaultColor;
	Color m_clickedColor;
};