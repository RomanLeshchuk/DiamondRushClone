#pragma once

#include "raylib.h"

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <map>

#include "data_types.h"
#include "Entity.h"
#include "Photos.h"
#include "Cell.h"
#include "Sidebar.h"
#include "Entities.h"

class EventsHandler;

class World
{
public:
	World(
		Photos& worldPhotos,
		const EventsHandler& eventsHandler,
		const PlayerEntity::Data& playerData,
		const Coords& viewportSize,
		const Coords& updateSize,
		const Coords& windowSize,
		int sidebarWidth,
		int framesPerMove,
		const Coords& maxPlayerShift
	);

	void update();
	void draw();

	Cell& getCell(const Coords& cellPos);

	~World();

	const EventsHandler* eventsHandler;
	
	PlayerEntity* player = nullptr;

	Coords cellSize;

	Coords viewportSize;
	Coords viewportCoords;
	int sidebarWidth;
	Coords viewportMoveVec = Movement<1>::NONE;
	Coords maxPlayerShift;

	Coords updateSize;
	int framesPerMove;
	Coords pixelsPerMove;

	Photos* photos;

	int currentFrame = 0;

private:
	void init(const PlayerEntity::Data& playerData);

	template <size_t element>
	void resetStaticData();

	std::vector<Cell> m_matrix{};

	Coords m_mapSize{};

	Sidebar m_sidebar;
	std::vector<Text> m_gameOver;
	const Texture* m_background;
};