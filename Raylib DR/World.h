#pragma once

#include "raylib.h"

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <queue>

#include "data_types.h"
#include "Entity.h"
#include "Photos.h"
#include "Cell.h"
#include "Sidebar.h"
#include "Entities.h"

class EventsHandler;

enum class WorldSignal
{
	LOSE_LEVEL,
	COMPLETE_LEVEL,
	OPEN_CHEST_EMPTY,
	OPEN_CHEST_D10,
	OPEN_CHEST_D20,
	OPEN_CHEST_D50,
	OPEN_CHEST_H3,
	OPEN_CHEST_H5,
	OPEN_CHEST_H7,
	GAME_EVENT
};

class World
{
public:
	struct CheckpointData
	{
		std::vector<Cell> matrix{};
		PlayerEntity* player = nullptr;
		int frame = 0;
		Coords viewportCoords{};
		Coords viewportMoveVec = Movement<1>::NONE;
	};

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

	void setSignal(WorldSignal signal);
	WorldSignal getSignal();
	void resolveSignal();

	Cell& getCell(const Coords& cellPos, bool fromCheckpoint = false);

	void saveCheckpoint();
	void loadCheckpoint();

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

	CheckpointData m_checkpointData{};

	std::queue<WorldSignal> m_signals{};

	Coords m_mapSize{};

	Sidebar m_sidebar;
	const Texture* m_background;

	Text m_mainText;
	Text m_bottomText;
	std::vector<std::string> m_textsData;
};