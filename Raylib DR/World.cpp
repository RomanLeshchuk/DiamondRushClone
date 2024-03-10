#include "World.h"

#include "Entities.h"
#include "EventsHandler.h"

World::World(
	const Photos& worldPhotos,
	const EventsHandler* eventsHandler,
	const Coords& viewportSize,
	const Coords& updateSize,
	const Coords& windowSize,
	int sidebarWidth,
	int framesPerMove,
	const Coords& maxPlayerShift) :
	photos{ worldPhotos },
	eventsHandler{ eventsHandler },
	viewportSize{ viewportSize },
	updateSize{ updateSize },
	cellSize{ windowSize / (viewportSize * 2 + 1) },
	sidebarWidth{ sidebarWidth },
	framesPerMove{ framesPerMove },
	pixelsPerMove{ cellSize / framesPerMove },
	maxPlayerShift{ maxPlayerShift },
	m_sidebar{},
	m_gameOver{ "Game over", { (windowSize.x + sidebarWidth) / 2, windowSize.y / 2 }, 128, WHITE },
	m_background{ photos.getSimpleTexture("background") }
{
	Entity::world = this;

	const Image* mapImage = photos.getSimpleImage("map");
	Color* colors = LoadImageColors(*mapImage);

	m_mapSize = { mapImage->width, mapImage->height };

	m_matrix.reserve(m_mapSize.x * m_mapSize.y);
	for (int y = 0; y < m_mapSize.y; y++)
	{
		for (int x = 0; x < m_mapSize.x; x++)
		{
			int i = y * m_mapSize.x + x;
			const Color& color = colors[i];
			std::unique_ptr<Entity> entity = nullptr;

			if (color == Color{ 0, 0, 0, 255 })
			{
				entity = std::make_unique<WallEntity>(Coords{ x, y });
			}
			else if (color == Color{ 63, 63, 63, 255 })
			{
				entity = std::make_unique<WallHiddenWayEntity>(Coords{ x, y });
			}
			else if (color == Color{ 127, 127, 127, 255 })
			{
				entity = std::make_unique<WallWayEntity>(Coords{ x, y });
			}
			else if (color == Color{ 0, 0, 255, 255 })
			{
				viewportCoords = { x, y };
				entity = std::make_unique<PlayerEntity>(viewportCoords, &eventsHandler->playerMoveEventSource);
				player = dynamic_cast<PlayerEntity*>(entity.get());
				m_sidebar = Sidebar(this);
			}
			else if (color == Color{ 0, 255, 0, 255 })
			{
				entity = std::make_unique<BushEntity>(Coords{ x, y });
			}
			else if (color == Color{ 255, 0, 0, 255 })
			{
				entity = std::make_unique<RockEntity>(Coords{ x, y });
			}
			else if (color == Color{ 127, 127, 255, 255 })
			{
				entity = std::make_unique<DiamondEntity>(Coords{ x, y });
			}
			else
			{
				m_matrix.emplace_back();
				continue;
			}

			Cell cell{};
			cell.add(std::move(entity));
			m_matrix.push_back(std::move(cell));
		}
	}

	UnloadImageColors(colors);
}

void World::update()
{
	if (*player->getHealth() <= 0)
	{
		return;
	}

	std::vector<Entity*> updateContainer = { player };

	for (int y = std::min(viewportCoords.y + updateSize.y, m_mapSize.y - 1); y >= std::max(viewportCoords.y - updateSize.y, 0); y--)
	{
		for (int x = std::max(viewportCoords.x - updateSize.x, 0); x < std::min(viewportCoords.x + updateSize.x + 1, m_mapSize.x); x++)
		{
			for (const std::unique_ptr<Entity>& entityPtr : this->getCell({ x, y }))
			{
				if (entityPtr->getType() != Entity::Type::PLAYER)
				{
					updateContainer.push_back(entityPtr.get());
				}
			}
		}
	}

	for (Entity* entity : updateContainer)
	{
		if (entity)
		{
			entity->update();
		}
	}

	updateContainer = { player };

	for (int y = std::min(viewportCoords.y + updateSize.y, m_mapSize.y - 1); y >= std::max(viewportCoords.y - updateSize.y, 0); y--)
	{
		for (int x = std::max(viewportCoords.x - updateSize.x, 0); x < std::min(viewportCoords.x + updateSize.x + 1, m_mapSize.x); x++)
		{
			for (const std::unique_ptr<Entity>& entityPtr : this->getCell({ x, y }))
			{
				if (entityPtr->getType() != Entity::Type::PLAYER)
				{
					updateContainer.push_back(entityPtr.get());
				}
			}
		}
	}

	for (Entity* entity : updateContainer)
	{
		if (entity)
		{
			entity->resetWasUpdated();
		}
	}

	currentMove = (currentMove + 1) % std::numeric_limits<int>::max();
}

void World::draw()
{
	if (*player->getHealth() <= 0)
	{
		m_gameOver.draw();
		return;
	}

	std::vector<Entity*> drawContainer{};

	for (int y = std::min(viewportCoords.y + viewportSize.y + 1, m_mapSize.y - 1); y >= std::max(viewportCoords.y - viewportSize.y - 1, 0); y--)
	{
		for (int x = std::max(viewportCoords.x - viewportSize.x - 1, 0); x < std::min(viewportCoords.x + viewportSize.x + 2, m_mapSize.x); x++)
		{
			for (const std::unique_ptr<Entity>& entityPtr : this->getCell({ x, y }))
			{
				drawContainer.push_back(entityPtr.get());
			}
		}
	}

	std::sort(drawContainer.begin(), drawContainer.end(), [](const Entity* firstEntity, const Entity* secondEntity) -> bool
		{
			return firstEntity->getType() < secondEntity->getType();
		}
	);

	for (int y = -1; y <= viewportSize.y * 2 + 1; y++)
	{
		for (int x = -1; x <= viewportSize.x * 2 + 1; x++)
		{
			DrawTexturePro(
				*m_background,
				{ 0.0f, 0.0f, (float)m_background->width, (float)m_background->height },
				Rectangle{ (float)sidebarWidth + x * cellSize.x, (float)y * cellSize.y, (float)cellSize.x, (float)cellSize.y } - (viewportMoveVec * pixelsPerMove * (currentFrame + 1)),
				{ 0.0f, 0.0f },
				0.0f,
				WHITE
			);
		}
	}

	for (Entity* entity : drawContainer)
	{
		entity->draw();
	}

	m_sidebar.draw();

	currentFrame = (currentFrame + 1) % framesPerMove;
}

Cell& World::getCell(const Coords& cellPos)
{
	return m_matrix[cellPos.y * m_mapSize.x + cellPos.x];
}
