#pragma once

#include "raylib.h"

#include <vector>
#include <map>

#include "data_types.h"
#include "Photos.h"

template <typename ...EntityClass>
class RawWorld;
class World;

class Shadow;

class Entity
{
public:
	enum class Type
	{
		WALL,
		BUSH,
		CHECKPOINT,
		FINISH,
		SWITCHER,
		SWITCHING,

		ROCK,
		DIAMOND,

		SHADOW,
		PLAYER,

		BUSH_PARTICLES,
		DIAMOND_PARTICLES,
		WALL_WAY,
		WALL_HIDDEN_WAY
	};

	Entity(const Coords& entityCoords, Entity::Type type);

	virtual void update();
	virtual void draw();

	Entity::Type getType() const;

	virtual void resetWasUpdated();

	void destroy();
	void replace(std::unique_ptr<Entity> newEntity);

	static void resetStaticResources();

	Coords coords;

	virtual ~Entity();

	friend class World;

protected:
	Entity();

	inline static World* world = nullptr;

	Entity::Type type;
};

class UpdatableEntity : virtual public Entity
{
public:
	UpdatableEntity();

	virtual void update() override;

	virtual void resetWasUpdated() override;

protected:
	virtual void calcUpdateState();

	bool wasUpdated = false;
};

class DrawableEntity : virtual public Entity
{
public:
	DrawableEntity();

	virtual void draw() override = 0;

protected:
	virtual void calcDrawState();

	Coords drawOffset{};

	Pair<float> currentDrawableStretch = { 1.0f, 1.0f };
	Pair<float> currentDrawableOffset = { 0.0f, 0.0f };
	Pair<bool> currentDrawableFlip = { false, false };
	float currentDrawableRotation = 0.0f;
};

class TexturedEntity : virtual public DrawableEntity
{
public:
	TexturedEntity(const Photos::PreloadedTexture* texture);

	virtual void draw() override;

protected:
	TexturedEntity();
	
	const Photos::PreloadedTexture* currentTexture;
};

class AnimatedEntity : virtual public UpdatableEntity, virtual public DrawableEntity
{
public:
	AnimatedEntity(const Photos::PreloadedAnimation* animation);

	void setAnimation(const Photos::PreloadedAnimation* animation);

	virtual void update() override;
	virtual void draw() override;

protected:
	AnimatedEntity();

	const Photos::PreloadedAnimation* currentAnimation;

	int currentAnimationFrameId = 0;
	int currentAnimationFramesPerTexture = 0;

private:
	int m_lastMoveRemainder = 0;
};

class MovableEntity : virtual public UpdatableEntity
{
public:
	MovableEntity();

	virtual void move();
	virtual std::vector<Entity*> getSolidEntitiesInOffsetCell(const Coords& offset);

	Coords moveVec = Movement<1>::NONE;
};

class SmoothlyMovableEntity : virtual public MovableEntity, virtual public DrawableEntity
{
public:
	SmoothlyMovableEntity();

	virtual void move() override;
	virtual std::vector<Entity*> getSolidEntitiesInOffsetCell(const Coords& offset) override;

	Shadow* shadow = nullptr;

	virtual ~SmoothlyMovableEntity() override;

protected:
	virtual void calcDrawState() override;
};

class TemporaryEntity : virtual public UpdatableEntity
{
public:
	TemporaryEntity(int maxUpdates);

	virtual void update() override;

protected:
	TemporaryEntity();

	int updatesCounter = 0;
	int maxUpdates;
};

class TemporaryAnimatedEntity : virtual public AnimatedEntity, virtual public TemporaryEntity
{
public:
	TemporaryAnimatedEntity();

	virtual void update() override;
};

class FallingEntity : virtual public SmoothlyMovableEntity
{
public:
	FallingEntity();

	virtual void move() override;

	virtual bool push(char direction);

	int getFallHeight();

protected:
	virtual void calcUpdateState() override;
	virtual void calcDrawState() override;

	int fallHeight = 0;
	char staggeringLeft = 0;
	char staggeringRight = 0;

	static constexpr float staggeringRotation = 25.0f;
	static constexpr float staggeringTranslation = 0.5f;
	static constexpr char staggeringTurns = 10;
};

class FallingRotatableEntity : virtual public FallingEntity
{
public:
	FallingRotatableEntity();

	virtual bool push(char direction) override;

protected:
	virtual void calcUpdateState() override;
	virtual void calcDrawState() override;

	char currentRotationState = 0;
	char rollDirection = 0;
};