#include "Entity.h"

#include "World.h"
#include "Entities.h"

Entity::Entity() = default;

Entity::Entity(World* world, const Coords& entityCoords, Entity::Type type) :
	world{ world }, coords{ entityCoords }, type{ type }
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
	world->getCell(newEntity->coords).insert_or_assign(newEntity->getType(), std::move(newEntity));
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
		{ drawOffset.x + (currentDrawableOffset.x + (currentDrawableFlip.x ? -1.0f : 1.0f) * currentTexture->offset.x - (std::cos(rotatationRad) - std::sin(rotatationRad)) / 2 + 0.5f) * world->cellSize.x,
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
		{ drawOffset.x + (currentDrawableOffset.x + (currentDrawableFlip.x ? -1.0f : 1.0f) * currentAnimation->offset.x - (std::cos(rotatationRad) - std::sin(rotatationRad)) / 2 + 0.5f) * world->cellSize.x,
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

	World::Cell::iterator prevIt = world->getCell(coords).find(type);
	world->getCell(coords + moveVec).insert_or_assign(prevIt->first, std::move(prevIt->second));
	world->getCell(coords).erase(prevIt);

	coords += moveVec;
}

std::vector<Entity*> MovableEntity::getSolidEntitiesInOffsetCell(const Coords& offset)
{
	std::vector<Entity*> solidEntities{};

	World::Cell& targetCell = world->getCell(coords + offset);
	for (World::Cell::iterator it = targetCell.begin(); it != targetCell.end(); it++)
	{
		if (it->first < Entity::Type::BUSH_PARTICLES)
		{
			solidEntities.push_back(it->second.get());
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

	World::Cell::iterator prevIt = world->getCell(coords).find(type);
	world->getCell(coords + moveVec).insert_or_assign(prevIt->first, std::move(prevIt->second));
	world->getCell(coords).erase(prevIt);

	std::unique_ptr<Shadow> entityShadow = std::make_unique<Shadow>(world, coords, this);
	entityShadow->update();
	shadow = entityShadow.get();
	world->getCell(coords).insert_or_assign(entityShadow->getType(), std::move(entityShadow));

	coords += moveVec;
}

std::vector<Entity*> SmoothlyMovableEntity::getSolidEntitiesInOffsetCell(const Coords& offset)
{
	std::vector<Entity*> solidEntities{};

	World::Cell& targetCell = world->getCell(coords + offset);
	for (World::Cell::iterator it = targetCell.begin(); it != targetCell.end(); it++)
	{
		if (it->first < Entity::Type::BUSH_PARTICLES)
		{
			if (it->first == Entity::Type::SHADOW)
			{
				Shadow* shadowTextured;
				if (it->second->getType() == Entity::Type::SHADOW && (shadowTextured = dynamic_cast<Shadow*>(it->second.get())) && offset.isCovering(shadowTextured->shadowOf->moveVec))
				{
					continue;
				}
			}

			solidEntities.push_back(it->second.get());
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

bool FallingEntity::push(char pushDirection)
{
	if (moveVec != Movement<1>::NONE
		|| !this->getSolidEntitiesInOffsetCell({ pushDirection, 0 }).empty())
	{
		return false;
	}

	this->moveVec = { pushDirection, 0 };
	this->move();

	return true;
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

	if (downCellSolidEntities.empty())
	{
		moveVec = Movement<1>::DOWN;
		fallHeigth++;
		staggeringLeft = 0;
		staggeringRight = 0;
	}
	else
	{
		fallHeigth = 0;
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
		return true;
	}

	return false;
}

void FallingRotatableEntity::calcUpdateState()
{
	if (staggeringLeft == -1 || rollDirection == -1)
	{
		currentRotationState = currentRotationState - 1 >= 0 ? currentRotationState - 1 : 3;
	}
	else if (staggeringRight == -1 || rollDirection == 1)
	{
		currentRotationState = (currentRotationState + 1) % 4;
	}

	rollDirection = 0;

	this->FallingEntity::calcUpdateState();
}

void FallingRotatableEntity::calcDrawState()
{
	this->FallingEntity::calcDrawState();

	if (staggeringLeft != -1 && staggeringRight != -1)
	{
		currentDrawableRotation += 90.0f * currentRotationState;
	}
	else if (staggeringLeft == -1)
	{
		currentDrawableRotation += 90.0f * (currentRotationState - (world->currentFrame + 1.0f) / world->framesPerMove);
	}
	else if (staggeringRight == -1)
	{
		currentDrawableRotation += 90.0f * (currentRotationState + (world->currentFrame + 1.0f) / world->framesPerMove);
	}

	currentDrawableRotation += rollDirection * 90.0f * (world->currentFrame + 1.0f) / world->framesPerMove;
}