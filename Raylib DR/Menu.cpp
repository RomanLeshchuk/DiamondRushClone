#include "Menu.h"

Menu::Menu(Photos& photos, const EventsHandler& eventsHandler, const Coords& size) :
	m_size{ size }, m_texture{ photos.getSimpleTexture("menu") }, m_eventsHandler{ &eventsHandler }
{
	this->setState(Menu::State::MENU);
}

void Menu::setState(Menu::State state)
{
	m_state = state;

	const int defaultFontSize = m_size.y / 18;
	switch (state)
	{
	case Menu::State::MENU:
		m_texts = {
			{ "Menu", Coords{ m_size.x / 2, (int)(m_size.y / 10.0f) }, defaultFontSize * 2, BLACK }
		};
		m_buttons = {
			{ Text("Start game", Coords{ m_size.x / 2, (int)(m_size.y / 2.0f) - defaultFontSize }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Exit", Coords{ m_size.x / 2, (int)(m_size.y / 2.0f) + defaultFontSize }, defaultFontSize, BLACK), DARKGRAY, RED }
		};
		break;

	case Menu::State::PAUSE:
		m_texts = {
			{ "Pause", Coords{ m_size.x / 2, (int)(m_size.y / 10.0f) }, defaultFontSize * 2, BLACK },
			{ "Player stats:", Coords{ m_size.x / 2, (int)(m_size.y / 2.0f) }, (int)(defaultFontSize * 1.5f), BLACK },
			{ "Diamonds: " + std::to_string(m_playerData.diamondsCollected), Coords{ m_size.x / 2, (int)(m_size.y / 1.4f) - defaultFontSize }, defaultFontSize, BLACK },
			{ "Health: " + std::to_string(m_playerData.health), Coords{ m_size.x / 2, (int)(m_size.y / 1.4f) + defaultFontSize }, defaultFontSize, BLACK }
		};
		m_buttons = {
			{ Text("Continue", Coords{ m_size.x / 2, (int)(m_size.y / 3.4f) - defaultFontSize }, defaultFontSize, BLACK), DARKGRAY, GRAY },
			{ Text("Exit to menu", Coords{ m_size.x / 2, (int)(m_size.y / 3.4f) + defaultFontSize }, defaultFontSize, BLACK), DARKGRAY, RED }
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

	for (int i = 0; i < m_buttons.size(); i++)
	{
		if (m_buttons[i].isClicked(*m_eventsHandler))
		{
			switch (i)
			{
			case 0:
				if (m_state == Menu::State::MENU)
				{
					return Menu::Signal::NEW_GAME;
				}
				else
				{
					return Menu::Signal::CONTINUE;
				}

			case 1:
				if (m_state == Menu::State::MENU)
				{
					return Menu::Signal::EXIT;
				}
				else
				{
					return Menu::Signal::EXIT_TO_MENU;
				}

			default:
				return Menu::Signal::NONE;
			}
		}
		m_buttons[i].draw();
	}

	return Menu::Signal::NONE;
}

void Menu::rebindPhotos(Photos& photos)
{
	m_texture = photos.getSimpleTexture("menu");
}