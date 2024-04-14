#include "EventsHandler.h"

#include "Button.h"

EventsHandler::EventsHandler(const Coords& eventsHandleRectPos, const Coords& eventsHandleRectSize) :
	m_handleRectPos{ eventsHandleRectPos }, m_handleRectSize{ eventsHandleRectSize }
{
}

void EventsHandler::update()
{
	m_gesture = GetGestureDetected();
	m_touchPos = GetTouchPosition(0);
}

void EventsHandler::handleEvents()
{
	Vector2 eventsTouchPos = { m_touchPos.x - m_handleRectPos.x, m_touchPos.y - m_handleRectPos.y };

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
		switch (m_gesture)
		{
		case GESTURE_TAP:
		case GESTURE_DOUBLETAP:
		case GESTURE_HOLD:
		case GESTURE_DRAG:
			if (eventsTouchPos.x < 0 || eventsTouchPos.y < 0 || eventsTouchPos.x >= m_handleRectSize.x || eventsTouchPos.y >= m_handleRectSize.y)
			{
				playerMoveEventSource = Movement<1>::NONE;
				break;
			}

			if (eventsTouchPos.x / m_handleRectSize.x > eventsTouchPos.y / m_handleRectSize.y)
			{
				if (1.0f - eventsTouchPos.x / m_handleRectSize.x > eventsTouchPos.y / m_handleRectSize.y)
				{
					playerMoveEventSource = Movement<1>::UP;
				}
				else
				{
					playerMoveEventSource = Movement<1>::RIGHT;
				}
			}
			else
			{
				if (1.0f - eventsTouchPos.x / m_handleRectSize.x > eventsTouchPos.y / m_handleRectSize.y)
				{
					playerMoveEventSource = Movement<1>::LEFT;
				}
				else
				{
					playerMoveEventSource = Movement<1>::DOWN;
				}
			}

			break;

		default:
			playerMoveEventSource = Movement<1>::NONE;
			enterEventSource = true;
		}
	}

	if (IsKeyDown(KEY_ENTER))
	{
		enterEventSource = true;
	}
	else
	{
		switch (m_gesture)
		{
		case GESTURE_SWIPE_UP:
			enterEventSource = true;
			break;

		default:
			enterEventSource = false;
		}
	}

	if (IsKeyDown(KEY_P))
	{
		pauseEventSource = true;
	}
	else
	{
		switch (m_gesture)
		{
		case GESTURE_SWIPE_DOWN:
			pauseEventSource = true;
			break;

		default:
			pauseEventSource = false;
		}
	}
}

std::pair<bool, Coords> EventsHandler::handleTouch() const
{
	bool res = false;

	switch (m_gesture)
	{
	case GESTURE_TAP:
		res = true;
		break;
	}

	return { res, Coords{ (int)m_touchPos.x, (int)m_touchPos.y } };
}