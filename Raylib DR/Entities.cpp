#include "Entities.h"

#include "Photos.h"
#include "World.h"

std::vector<const Photos::PreloadedAnimation*> PlayerEntity::m_animationsList{};

PlayerEntity::PlayerEntity(const Coords& entityCoords, const Coords* moveEventSource, const Data& playerData) :
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
			world->photos->getAnimation("player_calm"),
			world->photos->getAnimation("player_push"),
			world->photos->getAnimation("player_hold")
		};
	}
	currentAnimation = m_animationsList[(int)Animations::CALM];
}

PlayerEntity* PlayerEntity::copyImpl() const
{
	return new PlayerEntity(*this);
}

void PlayerEntity::changeDiamonds(int value)
{
	m_data.diamondsCollected += value;
}

void PlayerEntity::changeHealth(int value)
{
	m_data.health = std::max(m_data.health + value, 0);
}

const PlayerEntity::Data& PlayerEntity::getData()
{
	return m_data;
}

void PlayerEntity::resetStaticResources()
{
	m_animationsList = {};
}

void PlayerEntity::calcUpdateState()
{
	this->SmoothlyMovableEntity::calcUpdateState();

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
		Entity* solidEntity = this->getSolidEntityInOffsetCell(moveVec);

		if (!solidEntity
			|| (solidEntity->getType() == Entity::Type::BUSH || solidEntity->getType() == Entity::Type::DIAMOND || solidEntity->getType() == Entity::Type::SHADOW))
		{
			this->setAnimation(m_animationsList[(int)Animations::CALM]);

			do
			{
				if (solidEntity)
				{
					if (solidEntity->getType() == Entity::Type::BUSH)
					{
						solidEntity->replace(std::make_unique<BushParticlesEntity>(solidEntity->coords));
					}
					else if (solidEntity->getType() == Entity::Type::DIAMOND)
					{
						solidEntity->replace(std::make_unique<DiamondParticlesEntity>(solidEntity->coords));
						m_data.diamondsCollected++;
					}
					else if (solidEntity->getType() == Entity::Type::SHADOW)
					{
						Shadow* shadow = dynamic_cast<Shadow*>(solidEntity);
						if (shadow->shadowOf->getType() == Entity::Type::DIAMOND)
						{
							shadow->shadowOf->replace(std::make_unique<DiamondParticlesEntity>(solidEntity->coords));
							m_data.diamondsCollected++;
						}
						else
						{
							moveVec = Movement<1>::NONE;
							world->viewportMoveVec = Movement<1>::NONE;

							break;
						}
					}
				}

				this->move();

				m_pushingTurn = 0;
			} while (false);	
		}
		else if (moveVec.x != 0 && solidEntity)
		{
			this->setAnimation(m_animationsList[(int)Animations::PUSHING]);

			do
			{
				FallingEntity* entityToPush;
				if ((entityToPush = dynamic_cast<FallingEntity*>(solidEntity)))
				{
					if ((m_pushingTurn == turnsNeededToPush || ++m_pushingTurn == turnsNeededToPush) && entityToPush->push(moveVec.x))
					{
						solidEntity = nullptr;
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
			this->setAnimation(m_animationsList[(int)Animations::CALM]);

			moveVec = Movement<1>::NONE;
			world->viewportMoveVec = Movement<1>::NONE;
			m_pushingTurn = 0;
		}
	}
	else
	{
		Entity* aboveEntity = this->getSolidEntityInOffsetCell(Movement<1>::UP);
		if (aboveEntity && aboveEntity->getType() == Entity::Type::ROCK)
		{
			this->setAnimation(m_animationsList[(int)Animations::HOLDING]);
		}
		else
		{
			this->setAnimation(m_animationsList[(int)Animations::CALM]);
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

Shadow* Shadow::copyImpl() const
{
	return new Shadow(*this);
}

Shadow::~Shadow()
{
	shadowOf->shadow = nullptr;
}

WallEntity::WallEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::WALL),
	DrawableEntity(),
	TexturedEntity(world->photos->getTexture("wall"))
{
}

WallEntity* WallEntity::copyImpl() const
{
	return new WallEntity(*this);
}

BushEntity::BushEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::BUSH),
	DrawableEntity(),
	TexturedEntity(world->photos->getTexture("bush"))
{
}

BushEntity* BushEntity::copyImpl() const
{
	return new BushEntity(*this);
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
			world->photos->getAnimation("bush_particles")
		};
	}
	currentAnimation = m_animationsList[0];
}

BushParticlesEntity* BushParticlesEntity::copyImpl() const
{
	return new BushParticlesEntity(*this);
}

void BushParticlesEntity::resetStaticResources()
{
	m_animationsList = {};
}

WallWayEntity::WallWayEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::WALL_WAY),
	DrawableEntity(),
	TexturedEntity(world->photos->getTexture("wall_way"))
{
}

WallWayEntity* WallWayEntity::copyImpl() const
{
	return new WallWayEntity(*this);
}

WallHiddenWayEntity::WallHiddenWayEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::WALL_HIDDEN_WAY),
	DrawableEntity(),
	TexturedEntity(world->photos->getTexture("wall"))
{
}

WallHiddenWayEntity* WallHiddenWayEntity::copyImpl() const
{
	return new WallHiddenWayEntity(*this);
}

RockEntity::RockEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::ROCK),
	DrawableEntity(),
	TexturedEntity(world->photos->getTexture("rock")),
	UpdatableEntity(),
	MovableEntity(),
	SmoothlyMovableEntity(),
	FallingEntity()
{
}

RockEntity* RockEntity::copyImpl() const
{
	return new RockEntity(*this);
}

void RockEntity::calcUpdateState()
{
	this->SmoothlyMovableEntity::calcUpdateState();

	if (coords + Movement<1>::DOWN == world->player->coords)
	{
		if (fallHeight)
		{
			world->player->changeHealth(-fallHeight);
			fallHeight = 0;
		}

		m_holdingTurn++;
		if (m_holdingTurn % 10 == 0)
		{
			world->player->changeHealth(-1);
		}
	}
	else
	{
		m_holdingTurn = 0;
	}

	this->FallingRotatableEntity::calcUpdateState();
}

DiamondEntity::DiamondEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::DIAMOND),
	DrawableEntity(),
	TexturedEntity(world->photos->getTexture("diamond")),
	UpdatableEntity(),
	MovableEntity(),
	SmoothlyMovableEntity(),
	FallingEntity()
{
}

DiamondEntity* DiamondEntity::copyImpl() const
{
	return new DiamondEntity(*this);
}

void DiamondEntity::calcUpdateState()
{
	this->SmoothlyMovableEntity::calcUpdateState();

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
			world->photos->getAnimation("diamond_particles")
		};
	}
	currentAnimation = m_animationsList[0];
}

DiamondParticlesEntity* DiamondParticlesEntity::copyImpl() const
{
	return new DiamondParticlesEntity(*this);
}

void DiamondParticlesEntity::resetStaticResources()
{
	m_animationsList = {};
}

FinishEntity::FinishEntity(const Coords& entityCoords) :
	Entity(entityCoords, Entity::Type::FINISH),
	DrawableEntity(),
	TexturedEntity(world->photos->getTexture("finish"))
{
}

FinishEntity* FinishEntity::copyImpl() const
{
	return new FinishEntity(*this);
}