#include "Entities.h"

#include "Photos.h"
#include "World.h"

std::vector<const Photos::PreloadedAnimation*> PlayerEntity::m_animationsList{};

PlayerEntity::PlayerEntity(const Coords& entityCoords, const Coords* moveEventSource) :
	Entity(entityCoords, Entity::Type::PLAYER),
	UpdatableEntity(),
	DrawableEntity(),
	AnimatedEntity(nullptr),
	MovableEntity(),
	SmoothlyMovableEntity(),
	m_moveEventSource{ moveEventSource }
{
	if (m_animationsList.empty())
	{
		m_animationsList = {
			world->photos.getAnimation("player_calm"),
			world->photos.getAnimation("player_push")
		};
	}
	currentAnimation = m_animationsList[(int)Animations::CALM];
}

void PlayerEntity::changeDiamonds(int value)
{
	m_diamondsCollected += value;
}

void PlayerEntity::changeHealth(int value)
{
	m_health += value;
}

const int* PlayerEntity::getDiamonds()
{
	return &m_diamondsCollected;
}

const int* PlayerEntity::getHealth()
{
	return &m_health;
}

void PlayerEntity::calcUpdateState()
{
	m_prevMoveVec = moveVec;
	moveVec = *m_moveEventSource;

	if (m_viewDirection != moveVec)
	{
		m_viewDirection = moveVec;

		if (m_viewDirection == Movement<1>::LEFT)
		{
			currentDrawableFlip.x = true;
		}
		else if (m_viewDirection == Movement<1>::RIGHT)
		{
			currentDrawableFlip.x = false;
		}

		if (m_prevMoveVec == Movement<1>::NONE)
		{
			moveVec = Movement<1>::NONE;
			world->viewportMoveVec = Movement<1>::NONE;
			m_pushingTurn = 0;
		}
	}

	if (moveVec)
	{
		std::vector<Entity*> solidEntities = this->getSolidEntitiesInOffsetCell(moveVec);

		if (solidEntities.empty()
			|| (solidEntities.size() == 1
			&& (solidEntities[0]->getType() == Entity::Type::BUSH || solidEntities[0]->getType() == Entity::Type::DIAMOND || solidEntities[0]->getType() == Entity::Type::SHADOW)))
		{
			if (!Photos::equalAnimations(currentAnimation, m_animationsList[(int)Animations::CALM]))
			{
				this->setNewAnimation(m_animationsList[(int)Animations::CALM]);
			}

			if (solidEntities.size() == 1)
			{
				if (solidEntities[0]->getType() == Entity::Type::BUSH)
				{
					solidEntities[0]->replace(std::make_unique<BushParticlesEntity>(solidEntities[0]->coords));
				}
				else if (solidEntities[0]->getType() == Entity::Type::DIAMOND)
				{
					solidEntities[0]->replace(std::make_unique<DiamondParticlesEntity>(solidEntities[0]->coords));
					m_diamondsCollected++;
				}
				else if (solidEntities[0]->getType() == Entity::Type::SHADOW)
				{
					Shadow* shadow = dynamic_cast<Shadow*>(solidEntities[0]);
					if (shadow->shadowOf->getType() == Entity::Type::DIAMOND && !shadow->shadowOf->moveVec.isCovering(moveVec))
					{
						shadow->shadowOf->replace(std::make_unique<DiamondParticlesEntity>(solidEntities[0]->coords));
						m_diamondsCollected++;
					}
				}
			}

			this->move();

			m_pushingTurn = 0;
		}
		else if (moveVec.x != 0 && solidEntities.size() == 1)
		{
			if (!Photos::equalAnimations(currentAnimation, m_animationsList[(int)Animations::PUSHING]))
			{
				this->setNewAnimation(m_animationsList[(int)Animations::PUSHING]);
			}

			do
			{
				FallingEntity* entityToPush;
				if (entityToPush = dynamic_cast<FallingEntity*>(solidEntities[0]))
				{
					if ((m_pushingTurn == turnsNeededToPush || ++m_pushingTurn == turnsNeededToPush) && entityToPush->push(moveVec.x))
					{
						solidEntities.clear();
						this->move();

						break;
					}

					moveVec = Movement<1>::NONE;
					world->viewportMoveVec = Movement<1>::NONE;

					break;
				}

				moveVec = Movement<1>::NONE;
				world->viewportMoveVec = Movement<1>::NONE;
				m_pushingTurn = 0;
			} while (false);
		}
		else
		{
			if (!Photos::equalAnimations(currentAnimation, m_animationsList[(int)Animations::CALM]))
			{
				this->setNewAnimation(m_animationsList[(int)Animations::CALM]);
			}

			moveVec = Movement<1>::NONE;
			world->viewportMoveVec = Movement<1>::NONE;
			m_pushingTurn = 0;
		}
	}
	else
	{
		if (!Photos::equalAnimations(currentAnimation, m_animationsList[(int)Animations::CALM]))
		{
			this->setNewAnimation(m_animationsList[(int)Animations::CALM]);
		}
	}

	const Coords& possibleShift = m_shift + moveVec;
	if (possibleShift.x < -world->maxPlayerShift.x
		|| possibleShift.x > world->maxPlayerShift.x
		|| possibleShift.y < -world->maxPlayerShift.y
		|| possibleShift.y > world->maxPlayerShift.y)
	{
		world->viewportCoords += moveVec;
		world->viewportMoveVec = moveVec;
	}
	else
	{
		m_shift += moveVec;
		world->viewportMoveVec = Movement<1>::NONE;
	}
}

Shadow::Shadow(const Coords& entityCoords, SmoothlyMovableEntity* const entityShadowOf) :
	Entity(entityCoords, Entity::Type::SHADOW),
	UpdatableEntity(),
	TemporaryEntity(1),
	shadowOf{ entityShadowOf }
{
}

Shadow::~Shadow()
{
	shadowOf->shadow = nullptr;
}

WallEntity::WallEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::WALL),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("wall"))
{
}

BushEntity::BushEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::BUSH),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("bush"))
{
}

std::vector<const Photos::PreloadedAnimation*> BushParticlesEntity::m_animationsList{};

BushParticlesEntity::BushParticlesEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::BUSH_PARTICLES),
	UpdatableEntity(),
	DrawableEntity(),
	AnimatedEntity(nullptr),
	TemporaryEntity(8),
	TemporaryAnimatedEntity()
{
	if (m_animationsList.empty())
	{
		m_animationsList = {
			world->photos.getAnimation("bush_particles")
		};
	}
	currentAnimation = m_animationsList[0];
}

WallWayEntity::WallWayEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::WALL_WAY),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("wall_way"))
{
}

WallHiddenWayEntity::WallHiddenWayEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::WALL_HIDDEN_WAY),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("wall"))
{
}

RockEntity::RockEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::ROCK),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("rock")),
	UpdatableEntity(),
	MovableEntity(),
	SmoothlyMovableEntity(),
	FallingEntity()
{
}

void RockEntity::calcUpdateState()
{
	if (fallHeight && coords + Movement<1>::DOWN == world->player->coords)
	{
		world->player->changeHealth(-fallHeight);
		fallHeight = 0;
	}

	this->FallingRotatableEntity::calcUpdateState();
}

DiamondEntity::DiamondEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::DIAMOND),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("diamond")),
	UpdatableEntity(),
	MovableEntity(),
	SmoothlyMovableEntity(),
	FallingEntity()
{
}

void DiamondEntity::calcUpdateState()
{
	if (fallHeight && coords + Movement<1>::DOWN == world->player->coords)
	{
		world->player->changeDiamonds(1);
		this->replace(std::make_unique<DiamondParticlesEntity>(world->player->coords));
		return;
	}

	this->FallingRotatableEntity::calcUpdateState();
}

std::vector<const Photos::PreloadedAnimation*> DiamondParticlesEntity::m_animationsList{};

DiamondParticlesEntity::DiamondParticlesEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::DIAMOND_PARTICLES),
	UpdatableEntity(),
	DrawableEntity(),
	AnimatedEntity(nullptr),
	TemporaryEntity(1),
	TemporaryAnimatedEntity()
{
	if (m_animationsList.empty())
	{
		m_animationsList = {
			world->photos.getAnimation("diamond_particles")
		};
	}
	currentAnimation = m_animationsList[0];
}