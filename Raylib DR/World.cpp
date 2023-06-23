#include "World.h"

#include "Entities.h"
#include "EventsHandler.h"
// #include <chrono>

World::World() = default;

World::World(
	const Photos& worldPhotos,
	const EventsHandler* eventsHandler,
	const Coords& viewportSize,
	const Coords& updateSize,
	const Coords& windowSize,
	int framesPerMove,
	const Coords& maxPlayerShift) :
	photos{ worldPhotos },
	eventsHandler{ eventsHandler },
	viewportSize{ viewportSize },
	updateSize{ updateSize },
	cellSize{ windowSize / (viewportSize * 2 + 1) },
	framesPerMove{ framesPerMove },
	pixelsPerMove{ cellSize / framesPerMove },
	maxPlayerShift{ maxPlayerShift }
{
	m_background = photos.getSimpleTexture("background");

	const Image* mapImage = photos.getSimpleImage("map");
	Color* colors = LoadImageColors(*mapImage);

	m_mapSize = { mapImage->width, mapImage->height };

	m_matrix = new Cell[m_mapSize.x * m_mapSize.y];
	for (int x = 0; x < m_mapSize.x; x++)
	{
		for (int y = 0; y < m_mapSize.y; y++)
		{
			int i = y * m_mapSize.x + x;
			const Color& color = colors[i];
			std::unique_ptr<Entity> entity = nullptr;

			if (color == Color{ 0, 0, 0, 255 })
			{
				entity = std::make_unique<WallEntity>(this, Coords{ x, y });
			}
			else if (color == Color{ 63, 63, 63, 255 })
			{
				entity = std::make_unique<WallHiddenWayEntity>(this, Coords{ x, y });
			}
			else if (color == Color{ 127, 127, 127, 255 })
			{
				entity = std::make_unique<WallWayEntity>(this, Coords{ x, y });
			}
			else if (color == Color{ 0, 0, 255, 255 })
			{
				viewportCoords = { x, y };
				entity = std::make_unique<PlayerEntity>(this, viewportCoords, &eventsHandler->playerMoveEventSource);
				player = dynamic_cast<PlayerEntity*>(entity.get());
			}
			else if (color == Color{ 0, 255, 0, 255 })
			{
				entity = std::make_unique<BushEntity>(this, Coords{ x, y });
			}
			else if (color == Color{ 255, 0, 0, 255 })
			{
				entity = std::make_unique<RockEntity>(this, Coords{ x, y });
			}
			else if (color == Color{ 127, 127, 255, 255 })
			{
				entity = std::make_unique<DiamondEntity>(this, Coords{ x, y });
			}
			else
			{
				m_matrix[i] = Cell{};
				continue;
			}

			Cell cell{};
			cell.emplace(entity->getType(), std::move(entity));
			m_matrix[i] = std::move(cell);
		}
	}

	UnloadImageColors(colors);
}

void World::update()
{
	for (int y = std::min(viewportCoords.y + updateSize.y, m_mapSize.y - 1); y >= std::max(viewportCoords.y - updateSize.y, 0); y--)
	{
		for (int x = std::max(viewportCoords.x - updateSize.x, 0); x < std::min(viewportCoords.x + updateSize.x + 1, m_mapSize.x); x++)
		{
			for (const std::pair<const Entity::Type, std::unique_ptr<Entity>>& entityPair : this->getCell({ x, y }))
			{
				if (entityPair.second->getType() != Entity::Type::PLAYER)
				{
					entityPair.second->update();
				}
			}
		}
	}

	player->update();

	for (int y = std::min(viewportCoords.y + updateSize.y, m_mapSize.y - 1); y >= std::max(viewportCoords.y - updateSize.y, 0); y--)
	{
		for (int x = std::max(viewportCoords.x - updateSize.x, 0); x < std::min(viewportCoords.x + updateSize.x + 1, m_mapSize.x); x++)
		{
			for (const std::pair<const Entity::Type, std::unique_ptr<Entity>>& entityPair : this->getCell({ x, y }))
			{
				entityPair.second->resetWasUpdated();
			}
		}
	}

	currentMove = (currentMove + 1) % std::numeric_limits<int>::max();
}

void World::draw()
{
	DrawTextureQuad(
		*m_background,
		{ viewportSize.x * 2 + 3.0f, viewportSize.y * 2 + 3.0f },
		{ 0.0f, 0.0f },
		Rectangle{ -(float)cellSize.x, -(float)cellSize.y, (viewportSize.x * 2 + 3.0f) * cellSize.x, (viewportSize.y * 2 + 3.0f) * cellSize.y }
		- (viewportMoveVec * pixelsPerMove * (currentFrame + 1)),
		WHITE
	);

	std::vector<Entity*> sortedEntities{};

	for (int y = std::min(viewportCoords.y + viewportSize.y + 1, m_mapSize.y - 1); y >= std::max(viewportCoords.y - viewportSize.y - 1, 0); y--)
	{
		for (int x = std::max(viewportCoords.x - viewportSize.x - 1, 0); x < std::min(viewportCoords.x + viewportSize.x + 2, m_mapSize.x); x++)
		{
			for (const std::pair<const Entity::Type, std::unique_ptr<Entity>>& entityPair : this->getCell({ x, y }))
			{
				sortedEntities.push_back(entityPair.second.get());
			}
		}
	}

	std::sort(sortedEntities.begin(), sortedEntities.end(), [](const Entity* firstEntity, const Entity* secondEntity) -> bool
		{
			return firstEntity->getType() < secondEntity->getType();
		}
	);

	for (Entity* entity : sortedEntities)
	{
		entity->draw();
	}

	currentFrame = (currentFrame + 1) % framesPerMove;
}

World::Cell& World::getCell(const Coords& cellPos)
{
	return m_matrix[cellPos.y * m_mapSize.x + cellPos.x];
}

World::~World()
{
	delete[] m_matrix;
}