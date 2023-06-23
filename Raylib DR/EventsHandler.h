#pragma once

#include "raylib.h"

#include "data_types.h"

class EventsHandler
{
public:
	EventsHandler() = default;

	Coords playerMoveEventSource = Movement<1>::NONE;

	void handleEvents();
};