#pragma once

#include "data_types.h"
#include "Photos.h"
#include "Text.h"
#include "Button.h"
#include "Entities.h"

class Menu
{
public:
	enum class State
	{
		MENU,
		PAUSE
	};

	enum class Signal
	{
		EXIT,
		EXIT_TO_MENU,
		NEW_GAME,
		CONTINUE,
		NONE
	};

	Menu() = default;
	Menu(Photos& photos, const EventsHandler& eventsHandler, const Coords& size);

	void setState(State state);

	void rebindPhotos(Photos& newPhotos);

	void setPlayerData(const PlayerEntity::Data& playerData);
	PlayerEntity::Data getPlayerData();

	Signal draw();

private:
	PlayerEntity::Data m_playerData{};
	State m_state;
	Coords m_size;
	const Photos::PreloadedSimpleTexture* m_texture;
	const EventsHandler* m_eventsHandler;
	std::vector<Text> m_texts{};
	std::vector<Button> m_buttons{};
};