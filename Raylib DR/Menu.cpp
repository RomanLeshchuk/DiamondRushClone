#include "Menu.h"

Menu::Menu(Photos& photos, const EventsHandler& eventsHandler, const Coords& size) :
	m_size{ size }, m_texture{ photos.getSimpleTexture("menu") }, m_eventsHandler{ &eventsHandler }
{
	this->setState(Menu::State::START_MENU);
}

void Menu::setState(Menu::State state)
{
	m_state = state;

	const int defaultFontSize = m_size.y / 18;
	switch (state)
	{
	case Menu::State::START_MENU:
		m_texts = {
			{ "Diamond Rush", Coords{ m_size.x / 2, (int)(m_size.y / 10.0f) }, defaultFontSize * 2, BLACK }
		};
		m_counters = {};
		m_buttons = {
			{ Text("Start game", Coords{ m_size.x / 2, (int)(m_size.y / 2.0f) - defaultFontSize }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Exit", Coords{ m_size.x / 2, (int)(m_size.y / 2.0f) + defaultFontSize }, defaultFontSize, BLACK), DARKGRAY, RED }
		};
		break;

	case Menu::State::MENU:
		m_texts = {
			{ "Menu", Coords{ m_size.x / 2, (int)(m_size.y / 10.0f) }, defaultFontSize * 2, BLACK },
			{ "Player stats:", Coords{ m_size.x / 2, (int)(m_size.y / 1.6f) }, (int)(defaultFontSize * 1.5f), BLACK }
		};
		m_counters = {
			{ "Diamonds", &m_playerData.diamondsCollected, Coords{ m_size.x / 2, (int)(m_size.y / 1.35f) }, defaultFontSize, BLACK },
			{ "Health", &m_playerData.health, Coords{ m_size.x / 2, (int)(m_size.y / 1.35f) + (int)(defaultFontSize * 1.2f) }, defaultFontSize, BLACK }
		};
		m_buttons = {
			{ Text("New game", Coords{ m_size.x / 2, (int)(m_size.y / 2.6f) - defaultFontSize * 2 }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Continue from last level", Coords{ m_size.x / 2, (int)(m_size.y / 2.6f) }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Exit", Coords{ m_size.x / 2, (int)(m_size.y / 2.6f) + defaultFontSize * 2 }, defaultFontSize, BLACK), DARKGRAY, RED }
		};
		break;

	case Menu::State::PAUSE:
		m_texts = {
			{ "Pause", Coords{ m_size.x / 2, (int)(m_size.y / 10.0f) }, defaultFontSize * 2, BLACK },
			{ "Player stats:", Coords{ m_size.x / 2, (int)(m_size.y / 1.6f) }, (int)(defaultFontSize * 1.5f), BLACK }
		};
		m_counters = {
			{ "Diamonds", &m_playerData.diamondsCollected, Coords{ m_size.x / 2, (int)(m_size.y / 1.35f) }, defaultFontSize, BLACK },
			{ "Health", &m_playerData.health, Coords{ m_size.x / 2, (int)(m_size.y / 1.35f) + (int)(defaultFontSize * 1.2f) }, defaultFontSize, BLACK }
		};
		m_buttons = {
			{ Text("Continue", Coords{ m_size.x / 2, (int)(m_size.y / 4.5f) }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Save checkpoint", Coords{ m_size.x / 2, (int)(m_size.y / 4.5f) + (int)(defaultFontSize * 1.2f) }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Load checkpoint", Coords{ m_size.x / 2, (int)(m_size.y / 4.5f) + (int)(defaultFontSize * 2.4f) }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Restart level", Coords{ m_size.x / 2, (int)(m_size.y / 4.5f) + (int)(defaultFontSize * 3.6f) }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Exit to menu", Coords{ m_size.x / 2, (int)(m_size.y / 4.5f) + (int)(defaultFontSize * 4.8f) }, defaultFontSize, BLACK), DARKGRAY, RED }
		};
	}
}

void Menu::setPlayerData(const PlayerEntity::Data& playerData)
{
	m_playerData = playerData;
}

PlayerEntity::Data Menu::getPlayerData()
{
	return m_playerData;
}

Menu::Signal Menu::draw()
{
	DrawTexturePro(
		*m_texture,
		{ 0.0f, 0.0f, (float)m_texture->width, (float)m_texture->height },
		{ 0.0f, 0.0f, (float)m_size.x, (float)m_size.y },
		{ 0.0f, 0.0f },
		0.0f,
		WHITE
	);

	for (const Text& text : m_texts)
	{
		text.draw();
	}

	for (const Counter& counter : m_counters)
	{
		counter.draw();
	}

	int clickedBtn = -1;
	for (int i = 0; i < m_buttons.size(); i++)
	{
		if (m_buttons[i].isClicked(*m_eventsHandler))
		{
			clickedBtn = i;
		}
		m_buttons[i].draw();
	}

	if (clickedBtn != -1)
	{
		switch (m_state)
		{
		case Menu::State::START_MENU:
			switch (clickedBtn)
			{
			case 0:
				return Menu::Signal::NEW_GAME;

			case 1:
				return Menu::Signal::EXIT;

			default:
				return Menu::Signal::NONE;
			}

		case Menu::State::MENU:
			switch (clickedBtn)
			{
			case 0:
				return Menu::Signal::NEW_GAME;

			case 1:
				return Menu::Signal::LAST_LEVEL;

			case 2:
				return Menu::Signal::EXIT;

			default:
				return Menu::Signal::NONE;
			}

		case Menu::State::PAUSE:
			switch (clickedBtn)
			{
			case 0:
				return Menu::Signal::CONTINUE;

			case 1:
				return Menu::Signal::SAVE;

			case 2:
				return Menu::Signal::LOAD;

			case 3:
				return Menu::Signal::LAST_LEVEL;

			case 4:
				return Menu::Signal::EXIT_TO_MENU;

			default:
				return Menu::Signal::NONE;
			}

		default:
			return Menu::Signal::NONE;
		}
	}

	return Menu::Signal::NONE;
}

void Menu::rebindPhotos(Photos& photos)
{
	m_texture = photos.getSimpleTexture("menu");
}