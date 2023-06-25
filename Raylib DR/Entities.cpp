#include "Entities.h"

#include "Photos.h"
#include "World.h"

std::vector<const Photos::PreloadedAnimation*> PlayerEntity::m_animationsList{};

PlayerEntity::PlayerEntity(World* world, const Coords& entityCoords, const Coords* moveEventSource) :
	Entity(world, entityCoords, Entity::Type::PLAYER),
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

Coords PlayerEntity::getPrevMoveVec()
{
	return m_prevMoveVec;
}

void PlayerEntity::calcUpdateState()
{
	m_prevMoveVec = moveVec;
	moveVec = *m_moveEventSource;

	if (moveVec == Movement<1>::NONE)
	{
		if (!Photos::equalAnimations(currentAnimation, m_animationsList[(int)Animations::CALM]))
		{
			this->setNewAnimation(m_animationsList[(int)Animations::CALM]);
		}

		world->viewportMoveVec = Movement<1>::NONE;
		m_pushingTurn = 0;

		return;
	}

	if (m_viewDirection != moveVec)
	{
		m_viewDirection = moveVec;

		if (!Photos::equalAnimations(currentAnimation, m_animationsList[(int)Animations::CALM]))
		{
			this->setNewAnimation(m_animationsList[(int)Animations::CALM]);
		}

		if (m_viewDirection == Movement<1>::LEFT)
		{
			currentDrawableFlip.x = true;
		}
		else if (m_viewDirection == Movement<1>::RIGHT)
		{
			currentDrawableFlip.x = false;
		}

		moveVec = Movement<1>::NONE;
		world->viewportMoveVec = Movement<1>::NONE;
		m_pushingTurn = 0;

		return;
	}

	std::vector<Entity*> solidEntities = this->getSolidEntitiesInOffsetCell(moveVec);
	
	if (solidEntities.empty() || (solidEntities.size() == 1
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
				solidEntities[0]->replace(std::make_unique<BushParticlesEntity>(world, coords + moveVec));
			}
			else if (solidEntities[0]->getType() == Entity::Type::DIAMOND)
			{
				solidEntities[0]->replace(std::make_unique<DiamondParticlesEntity>(world, coords + moveVec));
				m_diamondsCollected++;
			}
			else if (solidEntities[0]->getType() == Entity::Type::SHADOW)
			{
				Shadow* shadow = dynamic_cast<Shadow*>(solidEntities[0]);
				if (shadow->shadowOf->getType() == Entity::Type::DIAMOND && !shadow->shadowOf->moveVec.isCovering(moveVec))
				{
					shadow->shadowOf->replace(std::make_unique<DiamondParticlesEntity>(world, coords + moveVec));
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
			if ((entityToPush = dynamic_cast<FallingEntity*>(solidEntities[0])))
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

		return;
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

Shadow::Shadow(World* world, const Coords& entityCoords, SmoothlyMovableEntity* const entityShadowOf) :
	Entity(world, entityCoords, Entity::Type::SHADOW),
	UpdatableEntity(),
	TemporaryEntity(1),
	shadowOf{ entityShadowOf }
{
}

Shadow::~Shadow()
{
	shadowOf->shadow = nullptr;
}

WallEntity::WallEntity(World* world, const Coords& entityCoords) :
	Entity(world, entityCoords, Entity::Type::WALL),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("wall"))
{
}

BushEntity::BushEntity(World* world, const Coords& entityCoords) :
	Entity(world, entityCoords, Entity::Type::BUSH),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("bush"))
{
}

std::vector<const Photos::PreloadedAnimation*> BushParticlesEntity::m_animationsList{};

BushParticlesEntity::BushParticlesEntity(World* world, const Coords& entityCoords) :
	Entity(world, entityCoords, Entity::Type::BUSH_PARTICLES),
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

WallWayEntity::WallWayEntity(World* world, const Coords& entityCoords) :
	Entity(world, entityCoords, Entity::Type::WALL_WAY),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("wall_way"))
{
}

WallHiddenWayEntity::WallHiddenWayEntity(World* world, const Coords& entityCoords) :
	Entity(world, entityCoords, Entity::Type::WALL_HIDDEN_WAY),
	DrawableEntity(),
	TexturedEntity(world->photos.getTexture("wall"))
{
}

RockEntity::RockEntity(World* world, const Coords& entityCoords) :
	Entity(world, entityCoords, Entity::Type::ROCK),
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
	if (fallHeigth > 1)
	{
		Cell& downCell = world->getCell(coords + Movement<1>::DOWN);
		Cell::iterator shadowIt;
		if (coords + Movement<1>::DOWN == world->player->coords
			|| coords + Movement<1>::DOWN == world->player->coords - world->player->getPrevMoveVec())
		{
			world->player->changeHealth(-fallHeigth);
		}
	}

	this->FallingRotatableEntity::calcUpdateState();
}

DiamondEntity::DiamondEntity(World* world, const Coords& entityCoords) :
	Entity(world, entityCoords, Entity::Type::DIAMOND),
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
	if (fallHeigth)
	{
		Cell& downCell = world->getCell(coords + Movement<1>::DOWN);
		Cell::iterator shadowIt;
		if (coords + Movement<1>::DOWN == world->player->coords
			|| coords + Movement<1>::DOWN == world->player->coords - world->player->getPrevMoveVec())
		{
			world->player->changeDiamonds(1);
			this->replace(std::make_unique<DiamondParticlesEntity>(world, coords + Movement<1>::DOWN));

			return;
		}
	}

	this->FallingRotatableEntity::calcUpdateState();
}

std::vector<const Photos::PreloadedAnimation*> DiamondParticlesEntity::m_animationsList{};

DiamondParticlesEntity::DiamondParticlesEntity(World* world, const Coords& entityCoords) :
	Entity(world, entityCoords, Entity::Type::DIAMOND_PARTICLES),
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