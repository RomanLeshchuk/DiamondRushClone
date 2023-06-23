#include "EventsHandler.h"

void EventsHandler::handleEvents()
{
	if (IsKeyDown(KEY_UP))
	{
		playerMoveEventSource = Movement<1>::UP;
	}
	else if (IsKeyDown(KEY_DOWN))
	{
		playerMoveEventSource = Movement<1>::DOWN;
	}
	else if (IsKeyDown(KEY_LEFT))
	{
		playerMoveEventSource = Movement<1>::LEFT;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		playerMoveEventSource = Movement<1>::RIGHT;
	}
	else
	{
		playerMoveEventSource = Movement<1>::NONE;
	}
}