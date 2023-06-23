#pragma once

#include "raylib.h"

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <memory>
#include <map>
#include <unordered_map>

#include "data_types.h"
#include "Entity.h"
#include "Photos.h"

class EventsHandler;
class PlayerEntity;

class World
{
public:
	using Cell = std::map<Entity::Type, std::unique_ptr<Entity>>;

	World();
	World(
		const Photos& worldPhotos,
		const EventsHandler* eventsHandler,
		const Coords& viewportSize,
		const Coords& updateSize,
		const Coords& windowSize,
		int framesPerMove,
		const Coords& maxPlayerShift
	);

	void update();
	void draw();

	Cell& getCell(const Coords& cellPos);

	const EventsHandler* eventsHandler;
	
	PlayerEntity* player = nullptr;

	Coords cellSize;

	Coords viewportSize;
	Coords viewportCoords;
	Coords viewportMoveVec = Movement<1>::NONE;
	Coords maxPlayerShift;

	Coords updateSize;
	int framesPerMove;
	Coords pixelsPerMove;

	Photos photos;

	int currentFrame = 0;
	int currentMove = 0;

	~World();

private:
	Cell* m_matrix;

	Coords m_mapSize{};

	const Texture* m_background;
};