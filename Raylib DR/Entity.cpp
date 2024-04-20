#include "Entity.h"

#include "World.h"
#include "Entities.h"

Entity::Entity() = default;

Entity::Entity(const Coords& entityCoords, Entity::Type type) :
	coords{ entityCoords }, type{ type }
{
}

std::unique_ptr<Entity> Entity::copy() const
{
	Entity* copyEntity = this->copyImpl();

	copyEntity->fromCheckpoint = !copyEntity->fromCheckpoint;

	return std::unique_ptr<Entity>(copyEntity);
}

bool Entity::update()
{
	return false;
}

void Entity::draw()
{
}

Entity::Type Entity::getType() const
{
	return type;
}

void Entity::resetWasUpdated()
{
}

void Entity::destroy()
{
	world->getCell(coords, fromCheckpoint).erase(type);
}

void Entity::replace(std::unique_ptr<Entity> newEntity)
{
	newEntity->update();

	Coords newEntityCoords = newEntity->coords;
	bool newEntityFromCheckpoint = newEntity->fromCheckpoint;
	world->getCell(newEntityCoords, newEntityFromCheckpoint).add(std::move(newEntity));
	
	this->destroy();
}

void Entity::resetStaticResources()
{
}

Entity::~Entity() = default;

UpdatableEntity::UpdatableEntity() = default;

bool UpdatableEntity::update()
{
	if (wasUpdated)
	{
		return false;
	}

	wasUpdated = true;

	this->calcUpdateState();

	return true;
}

void UpdatableEntity::resetWasUpdated()
{
	wasUpdated = false;
}

void UpdatableEntity::calcUpdateState()
{
}

DrawableEntity::DrawableEntity() = default;

void DrawableEntity::calcDrawState()
{
	drawOffset =
		(coords - world->viewportCoords + world->viewportSize) * world->cellSize
		+ world->viewportMoveVec * world->pixelsPerMove * (world->framesPerMove - (world->currentFrame + 1));
}

TexturedEntity::TexturedEntity() = default;

TexturedEntity::TexturedEntity(const Photos::PreloadedTexture* texture) :
	currentTexture{ texture }
{
}

void TexturedEntity::draw()
{
	this->calcDrawState();

	if (!currentTexture)
	{
		return;
	}

	float rotatationRad = currentDrawableRotation * ToRadians;
	DrawTexturePro(
		currentTexture->texture,
		{ 0.0f, 0.0f,
		((currentDrawableFlip.x != currentTexture->flip.x) ? -1.0f : 1.0f) * currentTexture->texture.width,
		((currentDrawableFlip.y != currentTexture->flip.y) ? -1.0f : 1.0f) * currentTexture->texture.height },
		{ world->sidebarWidth + drawOffset.x
		+ (currentDrawableOffset.x + (currentDrawableFlip.x ? -1.0f : 1.0f) * currentTexture->offset.x - (std::cos(rotatationRad) - std::sin(rotatationRad)) / 2 + 0.5f) * world->cellSize.x,
		drawOffset.y + (currentDrawableOffset.y + (currentDrawableFlip.y ? -1.0f : 1.0f) * currentTexture->offset.y - (std::cos(rotatationRad) + std::sin(rotatationRad)) / 2 + 0.5f) * world->cellSize.y,
		currentDrawableStretch.x * currentTexture->stretch.x * world->cellSize.x, currentDrawableStretch.y * currentTexture->stretch.y * world->cellSize.y },
		{ 0.0f, 0.0f },
		currentDrawableRotation,
		WHITE
	);
}

AnimatedEntity::AnimatedEntity() = default;

AnimatedEntity::AnimatedEntity(const Photos::PreloadedAnimation* animation) :
	currentAnimation{ animation }
{
}

void AnimatedEntity::setAnimation(const Photos::PreloadedAnimation* animation)
{
	if (Photos::equalAnimations(currentAnimation, animation))
	{
		return;
	}

	currentAnimation = animation;
	currentAnimationFrameId = 0;
	m_lastMoveRemainder = 0;
}

bool AnimatedEntity::update()
{
	if (wasUpdated)
	{
		return false;
	}

	wasUpdated = true;

	this->calcUpdateState();

	currentAnimationFramesPerTexture = std::max((int)(world->framesPerMove * currentAnimation->duration / currentAnimation->sequence.size()), 1);

	if (currentAnimationFrameId == currentAnimation->sequence.size())
	{
		currentAnimationFrameId = 0;
	}

	return true;
}

void AnimatedEntity::draw()
{
	this->calcDrawState();

	if (!currentAnimation)
	{
		return;
	}

	float rotatationRad = currentDrawableRotation * ToRadians;
	DrawTexturePro(
		currentAnimation->animation,
		{ (float)currentAnimation->frameWidth * (currentAnimation->sequence[currentAnimationFrameId] - 1), 0.0f,
		((currentDrawableFlip.x != currentAnimation->flip.x) ? -1.0f : 1.0f) * currentAnimation->frameWidth,
		((currentDrawableFlip.y != currentAnimation->flip.y) ? -1.0f : 1.0f) * currentAnimation->animation.height },
		{ world->sidebarWidth + drawOffset.x
		+ (currentDrawableOffset.x + (currentDrawableFlip.x ? -1.0f : 1.0f) * currentAnimation->offset.x - (std::cos(rotatationRad) - std::sin(rotatationRad)) / 2 + 0.5f) * world->cellSize.x,
		drawOffset.y + (currentDrawableOffset.y + (currentDrawableFlip.y ? -1.0f : 1.0f) * currentAnimation->offset.y - (std::cos(rotatationRad) + std::sin(rotatationRad)) / 2 + 0.5f) * world->cellSize.y,
		currentDrawableStretch.x * currentAnimation->stretch.x * world->cellSize.x, currentDrawableStretch.y * currentAnimation->stretch.y * world->cellSize.y },
		{ 0.0f, 0.0f },
		currentDrawableRotation,
		WHITE
	);

	int remainder = (m_lastMoveRemainder + world->currentFrame + 1) % currentAnimationFramesPerTexture;
	if (remainder == 0)
	{
		currentAnimationFrameId++;
	}

	if (world->currentFrame + 1 == world->framesPerMove)
	{
		m_lastMoveRemainder = remainder;
	}
}

MovableEntity::MovableEntity() = default;

void MovableEntity::move()
{
	if (moveVec == Movement<1>::NONE)
	{
		return;
	}

	Cell::iterator prevIt = world->getCell(coords).find(type);
	world->getCell(coords + moveVec).add(std::move(*prevIt));
	world->getCell(coords).erase(prevIt);

	coords += moveVec;
}

Entity* MovableEntity::getSolidEntityInOffsetCell(const Coords& offset)
{
	for (const std::unique_ptr<Entity>& entityPtr : world->getCell(coords + offset))
	{
		if (entityPtr->getType() > Entity::Type::FINISH && entityPtr->getType() < Entity::Type::BUSH_PARTICLES)
		{
			return entityPtr.get();
		}
	}

	return nullptr;
}

SmoothlyMovableEntity::SmoothlyMovableEntity() = default;

void SmoothlyMovableEntity::move()
{
	if (moveVec == Movement<1>::NONE)
	{
		return;
	}

	Entity* nextEntity = this->MovableEntity::getSolidEntityInOffsetCell(moveVec);
	if (nextEntity)
	{
		nextEntity->destroy();
	}

	Cell::iterator prevIt = world->getCell(coords).find(type);
	world->getCell(coords + moveVec).add(std::move(*prevIt));
	world->getCell(coords).erase(prevIt);

	std::unique_ptr<Shadow> entityShadow = std::make_unique<Shadow>(coords, this);
	entityShadow->update();
	shadow = entityShadow.get();
	world->getCell(coords).add(std::move(entityShadow));

	coords += moveVec;
}

Entity* SmoothlyMovableEntity::getSolidEntityInOffsetCell(const Coords& offset)
{
	Entity* anyShadow = nullptr;

	for (const std::unique_ptr<Entity>& entityPtr : world->getCell(coords + offset))
	{
		if (entityPtr->getType() > Entity::Type::FINISH && entityPtr->getType() < Entity::Type::BUSH_PARTICLES)
		{
			if (entityPtr->getType() == Entity::Type::SHADOW)
			{
				Shadow* shadow = dynamic_cast<Shadow*>(entityPtr.get());
				if (!shadow->shadowOf->moveVec.isCovering(offset))
				{
					anyShadow = shadow;
				}
			}
			else
			{
				return entityPtr.get();
			}
		}
	}

	return anyShadow;
}

void SmoothlyMovableEntity::calcUpdateState()
{
	if (shadow)
	{
		shadow->update();
	}
}

void SmoothlyMovableEntity::calcDrawState()
{
	this->DrawableEntity::calcDrawState();

	drawOffset -= moveVec * world->pixelsPerMove * (world->framesPerMove - (world->currentFrame + 1));
}

SmoothlyMovableEntity::~SmoothlyMovableEntity()
{
	if (shadow)
	{
		shadow->destroy();
	}
}

TemporaryEntity::TemporaryEntity() = default;

TemporaryEntity::TemporaryEntity(int maxUpdates) :
	maxUpdates{ maxUpdates }
{
}

bool TemporaryEntity::update()
{
	if (wasUpdated)
	{
		return false;
	}

	wasUpdated = true;

	if (updatesCounter++ == maxUpdates)
	{
		this->destroy();
		return true;
	}

	this->calcUpdateState();

	return true;
}

TemporaryAnimatedEntity::TemporaryAnimatedEntity() = default;

bool TemporaryAnimatedEntity::update()
{
	if (wasUpdated)
	{
		return false;
	}

	wasUpdated = true;

	if (updatesCounter++ == maxUpdates)
	{
		this->destroy();
		return true;
	}
	
	this->calcUpdateState();

	currentAnimationFramesPerTexture = std::max((int)(world->framesPerMove * currentAnimation->duration / currentAnimation->sequence.size()), 1);

	if (currentAnimationFrameId == currentAnimation->sequence.size())
	{
		currentAnimationFrameId = 0;
	}

	return true;
}

FallingEntity::FallingEntity() = default;

void FallingEntity::move()
{
	if (moveVec == Movement<1>::DOWN)
	{
		fallHeight++;
		staggeringLeft = 0;
		staggeringRight = 0;
	}
	else
	{
		fallHeight = 0;
	}

	this->SmoothlyMovableEntity::move();
}

bool FallingEntity::push(char direction)
{
	if (moveVec != Movement<1>::NONE
		|| this->getSolidEntityInOffsetCell({ direction, 0 })
		|| !this->getSolidEntityInOffsetCell(Movement<1>::DOWN))
	{
		return false;
	}

	staggeringLeft = 0;
	staggeringRight = 0;

	this->moveVec = { direction, 0 };
	this->move();

	wasUpdated = true;

	return true;
}

int FallingEntity::getFallHeight()
{
	return fallHeight;
}

void FallingEntity::calcUpdateState()
{
	moveVec = Movement<1>::NONE;

	Entity* downCellSolidEntity = this->getSolidEntityInOffsetCell(Movement<1>::DOWN);

	if (staggeringLeft == -1)
	{
		staggeringLeft = 0;
	}
	else if (staggeringRight == -1)
	{
		staggeringRight = 0;
	}

	if (!downCellSolidEntity)
	{
		moveVec = Movement<1>::DOWN;
	}
	else
	{
		if (downCellSolidEntity && downCellSolidEntity->getType() >= Entity::Type::ROCK && downCellSolidEntity->getType() <= Entity::Type::DIAMOND)
		{
			if (!this->getSolidEntityInOffsetCell(Movement<1>::LEFT) && !this->getSolidEntityInOffsetCell(Movement<1>::LEFT + Movement<1>::DOWN))
			{
				staggeringRight = 0;
				if (++staggeringLeft == 10)
				{
					moveVec = Movement<1>::LEFT;
					staggeringLeft = -1;
				}
			}
			else if (!this->getSolidEntityInOffsetCell(Movement<1>::RIGHT) && !this->getSolidEntityInOffsetCell(Movement<1>::RIGHT + Movement<1>::DOWN))
			{
				staggeringLeft = 0;
				if (++staggeringRight == 10)
				{
					moveVec = Movement<1>::RIGHT;
					staggeringRight = -1;
				}
			}
			else
			{
				if (!this->getSolidEntityInOffsetCell(Movement<1>::LEFT))
				{
					staggeringLeft = std::max(staggeringLeft - 2, 0);
				}
				else
				{
					staggeringLeft = 0;
				}

				if (!this->getSolidEntityInOffsetCell(Movement<1>::RIGHT))
				{
					staggeringRight = std::max(staggeringRight - 2, 0);
				}
				else
				{
					staggeringRight = 0;
				}
			}
		}
		else
		{
			if (!this->getSolidEntityInOffsetCell(Movement<1>::LEFT))
			{
				staggeringLeft = std::max(staggeringLeft - 2, 0);
			}
			else
			{
				staggeringLeft = 0;
			}

			if (!this->getSolidEntityInOffsetCell(Movement<1>::RIGHT))
			{
				staggeringRight = std::max(staggeringRight - 2, 0);
			}
			else
			{
				staggeringRight = 0;
			}
		}

		Entity* aboveLeftEntity = this->getSolidEntityInOffsetCell(Movement<1>::LEFT + Movement<1>::UP);
		if (staggeringLeft > 0 && aboveLeftEntity && aboveLeftEntity->getType() >= Entity::Type::ROCK && aboveLeftEntity->getType() <= Entity::Type::DIAMOND)
		{
			staggeringLeft = 0;
		}
	}

	this->move();
}

void FallingEntity::calcDrawState()
{
	this->SmoothlyMovableEntity::calcDrawState();

	if (world->currentFrame < world->framesPerMove / 2)
	{
		if (staggeringLeft > 0)
		{
			currentDrawableOffset.x = -staggeringLeft * staggeringTranslation / staggeringTurns;
		}
		else if (staggeringRight > 0)
		{
			currentDrawableOffset.x = staggeringRight * staggeringTranslation / staggeringTurns;
		}
		else
		{
			if (staggeringLeft == -1)
			{
				drawOffset.x -= (world->pixelsPerMove.x * (world->framesPerMove - (world->currentFrame + 1))) / 2;
			}
			else if (staggeringRight == -1)
			{
				drawOffset.x += (world->pixelsPerMove.x * (world->framesPerMove - (world->currentFrame + 1))) / 2;
			}

			currentDrawableOffset.x = 0.0f;
		}
	}
	else
	{
		if (staggeringLeft > 0)
		{
			currentDrawableOffset.x = -(staggeringLeft - 1) * staggeringTranslation / staggeringTurns;
		}
		else if (staggeringRight > 0)
		{
			currentDrawableOffset.x = (staggeringRight - 1) * staggeringTranslation / staggeringTurns;
		}
		else
		{
			if (staggeringLeft == -1)
			{
				drawOffset.x -= (world->pixelsPerMove.x * (world->framesPerMove - (world->currentFrame + 1))) / 2;
			}
			else if (staggeringRight == -1)
			{
				drawOffset.x += (world->pixelsPerMove.x * (world->framesPerMove - (world->currentFrame + 1))) / 2;
			}

			currentDrawableOffset.x = 0.0f;
		}
	}

	if (staggeringLeft != -1 && staggeringRight != -1)
	{
		currentDrawableRotation = currentDrawableOffset.x * staggeringRotation / staggeringTranslation;
	}
	else if (staggeringLeft == -1)
	{
		currentDrawableRotation = -staggeringRotation * (world->framesPerMove - world->currentFrame - 1) / world->framesPerMove;
	}
	else if (staggeringRight == -1)
	{
		currentDrawableRotation = staggeringRotation * (world->framesPerMove - world->currentFrame - 1) / world->framesPerMove;
	}
}

FallingRotatableEntity::FallingRotatableEntity() = default;

bool FallingRotatableEntity::push(char direction)
{
	if (this->FallingEntity::push(direction))
	{
		rollDirection = direction;
		if (rollDirection == -1)
		{
			currentRotationState = (currentRotationState - 1 >= 0 ? currentRotationState - 1 : 3);
		}
		else if (rollDirection == 1)
		{
			currentRotationState = (currentRotationState + 1) % 4;
		}

		return true;
	}

	return false;
}

void FallingRotatableEntity::calcUpdateState()
{
	this->FallingEntity::calcUpdateState();

	if (staggeringLeft == -1)
	{
		currentRotationState = currentRotationState - 1 >= 0 ? currentRotationState - 1 : 3;
		rollDirection = -1;
	}
	else if (staggeringRight == -1)
	{
		currentRotationState = (currentRotationState + 1) % 4;
		rollDirection = 1;
	}
	else
	{
		rollDirection = 0;
	}
}

void FallingRotatableEntity::calcDrawState()
{
	this->FallingEntity::calcDrawState();

	currentDrawableRotation += rollDirection * 90.0f * ((world->currentFrame + 1.0f) / world->framesPerMove);

	currentDrawableRotation += 90.0f * (currentRotationState - rollDirection);
}