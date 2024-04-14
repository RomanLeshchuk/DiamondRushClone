#pragma once

#include "raylib.h"

#include "data_types.h"

class Button;

class EventsHandler
{
public:
	EventsHandler() = default;
	EventsHandler(const Coords& eventsHandleRectPos, const Coords& eventsHandleRectSize);

	Coords playerMoveEventSource = Movement<1>::NONE;
	bool enterEventSource = false;
	bool pauseEventSource = false;


	void update();

	void handleEvents();
	std::pair<bool, Coords> handleTouch() const;

private:
	Coords m_handleRectPos{ -1, -1 };
	Coords m_handleRectSize{ -1, -1 };

	int m_gesture = -1;
	Vector2 m_touchPos{ -1, -1 };
};