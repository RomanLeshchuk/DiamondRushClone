#include "World.h"

#include "Entities.h"
#include "EventsHandler.h"

World::World(
	Photos& worldPhotos,
	const EventsHandler& eventsHandler,
	const PlayerEntity::Data& playerData,
	const Coords& viewportSize,
	const Coords& updateSize,
	const Coords& windowSize,
	int sidebarWidth,
	int framesPerMove,
	const Coords& maxPlayerShift) :
	photos{ &worldPhotos },
	eventsHandler{ &eventsHandler },
	viewportSize{ viewportSize },
	updateSize{ updateSize },
	cellSize{ windowSize / (viewportSize * 2 + 1) },
	sidebarWidth{ sidebarWidth },
	framesPerMove{ framesPerMove },
	pixelsPerMove{ cellSize / framesPerMove },
	maxPlayerShift{ maxPlayerShift },
	m_sidebar{},
	m_gameOver{
		{ "Game over", { sidebarWidth + windowSize.x / 2, windowSize.y / 2 }, windowSize.y / 10, WHITE },
		{ "Press [Enter] or [Swipe Up] to go to menu", { sidebarWidth + windowSize.x / 2, windowSize.y / 2 + windowSize.y / 5 }, windowSize.y / 25, WHITE }
	},
	m_background{ photos->getSimpleTexture("background") }
{
	this->init(playerData);
}

void World::init(const PlayerEntity::Data& playerData)
{
	Entity::world = this;

	const Image* mapImage = photos->getSimpleImage("map");
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
				entity = std::make_unique<PlayerEntity>(viewportCoords, &eventsHandler->playerMoveEventSource, playerData);
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
			else if (color == Color{ 255, 255, 0, 255 })
			{
				entity = std::make_unique<FinishEntity>(Coords{ x, y });
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

	this->saveCheckpoint();
}

void World::update()
{
	if (!player->getData().health)
	{
		return;
	}

	player->update();
	for (int y = std::min(viewportCoords.y + updateSize.y, m_mapSize.y - 1); y >= std::max(viewportCoords.y - updateSize.y, 0); y--)
	{
		for (int x = std::max(viewportCoords.x - updateSize.x, 0); x < std::min(viewportCoords.x + updateSize.x + 1, m_mapSize.x); x++)
		{
			Cell& cell = this->getCell({ x, y });
			bool updateCell = true;
			while (updateCell)
			{
				auto it = cell.begin();
				while (true)
				{
					if (it == cell.end())
					{
						updateCell = false;
						break;
					}
					else if ((*it)->getType() != Entity::Type::PLAYER && (*it)->update())
					{
						break;
					}
					it++;
				}
			}
		}
	}

	player->resetWasUpdated();
	for (int y = std::min(viewportCoords.y + updateSize.y, m_mapSize.y - 1); y >= std::max(viewportCoords.y - updateSize.y, 0); y--)
	{
		for (int x = std::max(viewportCoords.x - updateSize.x, 0); x < std::min(viewportCoords.x + updateSize.x + 1, m_mapSize.x); x++)
		{
			for (const std::unique_ptr<Entity>& entityPtr : this->getCell({ x, y }))
			{
				if (entityPtr->getType() != Entity::Type::PLAYER)
				{
					entityPtr->resetWasUpdated();
				}
			}
		}
	}
}

void World::draw()
{
	if (!player->getData().health)
	{
		m_sidebar.draw();

		for (const Text& text : m_gameOver)
		{
			text.draw();
		}
		
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

Cell& World::getCell(const Coords& cellPos, bool fromCheckpoint)
{
	return fromCheckpoint ? m_checkpointData.matrix[cellPos.y * m_mapSize.x + cellPos.x] : m_matrix[cellPos.y * m_mapSize.x + cellPos.x];
}

void World::saveCheckpoint()
{
	m_checkpointData.matrix.clear();
	m_checkpointData.matrix.resize(m_mapSize.x * m_mapSize.y);

	for (int i = 0; i < m_matrix.size(); i++)
	{
		for (const std::unique_ptr<Entity>& entity : m_matrix[i])
		{
			if (entity->type == Entity::Type::SHADOW)
			{
				continue;
			}

			std::unique_ptr<Entity> newEntity = entity->copy();
			Entity* newEntityPtr = newEntity.get();

			m_checkpointData.matrix[i].add(std::move(newEntity));

			SmoothlyMovableEntity* smoothEntity = dynamic_cast<SmoothlyMovableEntity*>(entity.get());
			if (smoothEntity && smoothEntity->shadow)
			{
				SmoothlyMovableEntity* newSmoothEntity = dynamic_cast<SmoothlyMovableEntity*>(newEntityPtr);

				std::unique_ptr<Entity> newShadowEntity = smoothEntity->shadow->copy();
				Shadow* newShadowPtr = dynamic_cast<Shadow*>(newShadowEntity.get());
				
				getCell(newShadowPtr->coords, true).add(std::move(newShadowEntity));

				newSmoothEntity->shadow = newShadowPtr;
				newShadowPtr->shadowOf = newSmoothEntity;
			}
		}
	}

	m_checkpointData.player = dynamic_cast<PlayerEntity*>(getCell(player->coords, true).find(Entity::Type::PLAYER)->get());
	m_checkpointData.frame = currentFrame;
	m_checkpointData.viewportCoords = viewportCoords;
	m_checkpointData.viewportMoveVec = viewportMoveVec;
}

void World::loadCheckpoint()
{
	m_matrix.clear();
	m_matrix.resize(m_mapSize.x * m_mapSize.y);

	for (int i = 0; i < m_checkpointData.matrix.size(); i++)
	{
		for (const std::unique_ptr<Entity>& entity : m_checkpointData.matrix[i])
		{
			if (entity->type == Entity::Type::SHADOW)
			{
				continue;
			}

			std::unique_ptr<Entity> newEntity = entity->copy();
			Entity* newEntityPtr = newEntity.get();

			m_matrix[i].add(std::move(newEntity));

			SmoothlyMovableEntity* smoothEntity = dynamic_cast<SmoothlyMovableEntity*>(entity.get());
			if (smoothEntity && smoothEntity->shadow)
			{
				SmoothlyMovableEntity* newSmoothEntity = dynamic_cast<SmoothlyMovableEntity*>(newEntityPtr);

				std::unique_ptr<Entity> newShadowEntity = smoothEntity->shadow->copy();
				Shadow* newShadowPtr = dynamic_cast<Shadow*>(newShadowEntity.get());

				getCell(newShadowPtr->coords).add(std::move(newShadowEntity));

				newSmoothEntity->shadow = newShadowPtr;
				newShadowPtr->shadowOf = newSmoothEntity;
			}
		}
	}

	player = dynamic_cast<PlayerEntity*>(getCell(m_checkpointData.player->coords).find(Entity::Type::PLAYER)->get());
	m_sidebar = Sidebar(this);
	currentFrame = m_checkpointData.frame;
	viewportCoords = m_checkpointData.viewportCoords;
	viewportMoveVec = m_checkpointData.viewportMoveVec;
}

template <>
void World::resetStaticData<0>()
{
	std::tuple_element_t<0, EntitiesClassesList>::resetStaticResources();
}

template <size_t element>
void World::resetStaticData()
{
	std::tuple_element_t<element, EntitiesClassesList>::resetStaticResources();
	this->resetStaticData<element - 1>();
}

World::~World()
{
	this->resetStaticData<std::tuple_size_v<EntitiesClassesList> - 1>();
}