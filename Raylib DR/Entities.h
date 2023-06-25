#pragma once

#include "raylib.h"

#include "data_types.h"
#include "Entity.h"

class PlayerEntity final : public SmoothlyMovableEntity, public AnimatedEntity
{
public:
	enum class Animations
	{
		CALM,
		PUSHING
	};

	PlayerEntity(World* world, const Coords& entityCoords, const Coords* moveEventSource);

	void changeDiamonds(int value);
	void changeHealth(int value);

	Coords getPrevMoveVec();

	FallingEntity* fallingAboveEntity = nullptr;

protected:
	virtual void calcUpdateState() override;

private:
	const Coords* m_moveEventSource;
	Coords m_shift = { 0, 0 };

	Coords m_viewDirection = Movement<1>::NONE;
	Coords m_prevMoveVec = Movement<1>::NONE;

	char m_pushingTurn = 0;

	int m_diamondsCollected = 0;
	int m_health = 10;


	static constexpr char turnsNeededToPush = 5;

	static std::vector<const Photos::PreloadedAnimation*> m_animationsList;
};

class Shadow final : public TemporaryEntity
{
public:
	Shadow(World* world, const Coords& entityCoords, SmoothlyMovableEntity* const entityShadowOf);

	SmoothlyMovableEntity* const shadowOf;

	virtual ~Shadow() override;
};

class WallEntity final : public TexturedEntity
{
public:
	WallEntity(World* world, const Coords& entityCoords);
};

class BushEntity final : public TexturedEntity
{
public:
	BushEntity(World* world, const Coords& entityCoords);
};

class BushParticlesEntity final : public TemporaryAnimatedEntity
{
public:
	BushParticlesEntity(World* world, const Coords& entityCoords);

private:
	static std::vector<const Photos::PreloadedAnimation*> m_animationsList;
};

class WallWayEntity final : public TexturedEntity
{
public:
	WallWayEntity(World* world, const Coords& entityCoords);
};

class WallHiddenWayEntity final : public TexturedEntity
{
public:
	WallHiddenWayEntity(World* world, const Coords& entityCoords);
};

class RockEntity final : public FallingRotatableEntity, public TexturedEntity
{
public:
	RockEntity(World* world, const Coords& entityCoords);

protected:
	virtual void calcUpdateState() override;
};

class DiamondEntity final : public FallingRotatableEntity, public TexturedEntity
{
public:
	DiamondEntity(World* world, const Coords& entityCoords);

protected:
	virtual void calcUpdateState() override;
};

class DiamondParticlesEntity final : public TemporaryAnimatedEntity
{
public:
	DiamondParticlesEntity(World* world, const Coords& entityCoords);

private:
	static std::vector<const Photos::PreloadedAnimation*> m_animationsList;
};