#include "Entity.h"

#include "World.h"
#include "Entities.h"

Entity::Entity() = default;

Entity::Entity(const Coords& entityCoords, Entity::Type type) :
	coords{ entityCoords }, type{ type }
{
}

void Entity::update()
{
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
	world->getCell(coords).erase(type);
}

void Entity::replace(std::unique_ptr<Entity> newEntity)
{
	newEntity->update();
	Coords newEntityCoords = newEntity->coords;
	world->getCell(newEntityCoords).add(std::move(newEntity));
	this->destroy();
}

Entity::~Entity() = default;

UpdatableEntity::UpdatableEntity() = default;

void UpdatableEntity::update()
{
	if (wasUpdated)
	{
		return;
	}

	wasUpdated = true;

	this->calcUpdateState();
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

void AnimatedEntity::setNewAnimation(const Photos::PreloadedAnimation* animation)
{
	currentAnimation = animation;
	currentAnimationFrameId = 0;
	m_lastMoveRemainder = 0;
}

void AnimatedEntity::update()
{
	if (wasUpdated)
	{
		return;
	}

	wasUpdated = true;

	this->calcUpdateState();

	currentAnimationFramesPerTexture = world->framesPerMove * currentAnimation->duration / currentAnimation->sequence.size();

	if (currentAnimationFrameId == currentAnimation->sequence.size())
	{
		currentAnimationFrameId = 0;
	}
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

std::vector<Entity*> MovableEntity::getSolidEntitiesInOffsetCell(const Coords& offset)
{
	std::vector<Entity*> solidEntities{};

	Cell& targetCell = world->getCell(coords + offset);
	for (const std::unique_ptr<Entity>& entityPtr : targetCell)
	{
		if (entityPtr->getType() < Entity::Type::BUSH_PARTICLES)
		{
			solidEntities.push_back(entityPtr.get());
		}
	}

	return solidEntities;
}

SmoothlyMovableEntity::SmoothlyMovableEntity() = default;

void SmoothlyMovableEntity::move()
{
	if (moveVec == Movement<1>::NONE)
	{
		return;
	}

	for (Entity* entity : this->MovableEntity::getSolidEntitiesInOffsetCell(moveVec))
	{
		entity->destroy();
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

std::vector<Entity*> SmoothlyMovableEntity::getSolidEntitiesInOffsetCell(const Coords& offset)
{
	std::vector<Entity*> solidEntities{};

	Cell& targetCell = world->getCell(coords + offset);
	for (const std::unique_ptr<Entity>& entityPtr : targetCell)
	{
		if (entityPtr->getType() < Entity::Type::BUSH_PARTICLES)
		{
			if (entityPtr->getType() == Entity::Type::SHADOW)
			{
				Shadow* shadow = dynamic_cast<Shadow*>(entityPtr.get());
				if (shadow->shadowOf->moveVec.isCovering(offset))
				{
					continue;
				}
			}

			solidEntities.push_back(entityPtr.get());
		}
	}

	return solidEntities;
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

void TemporaryEntity::update()
{
	if (wasUpdated)
	{
		return;
	}

	wasUpdated = true;

	if (updatesCounter++ == maxUpdates)
	{
		this->destroy();
		return;
	}

	this->calcUpdateState();
}

TemporaryAnimatedEntity::TemporaryAnimatedEntity() = default;

void TemporaryAnimatedEntity::update()
{
	if (wasUpdated)
	{
		return;
	}

	wasUpdated = true;

	if (updatesCounter++ == maxUpdates)
	{
		this->destroy();
		return;
	}
	
	this->calcUpdateState();

	currentAnimationFramesPerTexture = world->framesPerMove * currentAnimation->duration / currentAnimation->sequence.size();

	if (currentAnimationFrameId == currentAnimation->sequence.size())
	{
		currentAnimationFrameId = 0;
	}
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
		|| !this->getSolidEntitiesInOffsetCell({ direction, 0 }).empty()
		|| this->getSolidEntitiesInOffsetCell(Movement<1>::DOWN).empty())
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

	std::vector<Entity*> downCellSolidEntities = this->getSolidEntitiesInOffsetCell(Movement<1>::DOWN);

	if (staggeringLeft == -1)
	{
		staggeringLeft = 0;
	}
	else if (staggeringRight == -1)
	{
		staggeringRight = 0;
	}

	if (downCellSolidEntities.empty() || (fallHeight
		&& (coords + Movement<1>::DOWN == world->player->coords - world->player->moveVec)))
	{
		moveVec = Movement<1>::DOWN;
	}
	else
	{
		if (downCellSolidEntities.size() == 1 && downCellSolidEntities[0]->getType() >= Entity::Type::ROCK && downCellSolidEntities[0]->getType() <= Entity::Type::DIAMOND)
		{
			if (this->getSolidEntitiesInOffsetCell(Movement<1>::LEFT).empty() && this->getSolidEntitiesInOffsetCell({ -1, 1 }).empty())
			{
				staggeringRight = 0;
				if (++staggeringLeft == 10)
				{
					moveVec = Movement<1>::LEFT;
					staggeringLeft = -1;
				}
			}
			else if (this->getSolidEntitiesInOffsetCell(Movement<1>::RIGHT).empty() && this->getSolidEntitiesInOffsetCell({ 1, 1 }).empty())
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
				if (this->getSolidEntitiesInOffsetCell(Movement<1>::LEFT).empty())
				{
					staggeringLeft = std::max(staggeringLeft - 2, 0);
				}
				else
				{
					staggeringLeft = 0;
				}

				if (this->getSolidEntitiesInOffsetCell(Movement<1>::RIGHT).empty())
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
			if (this->getSolidEntitiesInOffsetCell(Movement<1>::LEFT).empty())
			{
				staggeringLeft = std::max(staggeringLeft - 2, 0);
			}
			else
			{
				staggeringLeft = 0;
			}

			if (this->getSolidEntitiesInOffsetCell(Movement<1>::RIGHT).empty())
			{
				staggeringRight = std::max(staggeringRight - 2, 0);
			}
			else
			{
				staggeringRight = 0;
			}
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