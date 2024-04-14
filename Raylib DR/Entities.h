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
		PUSHING,
		HOLDING
	};

	struct Data
	{
		int diamondsCollected = 0;
		int health = 10;
	};

	PlayerEntity(const Coords& entityCoords, const Coords* moveEventSource, const Data& playerData);

	void changeDiamonds(int value);
	void changeHealth(int value);

	const Data& getData();

	static void resetStaticResources();

protected:
	virtual void calcUpdateState() override;

private:
	const Coords* m_moveEventSource;
	Coords m_shift = { 0, 0 };

	Coords m_viewDirection = Movement<1>::NONE;
	Coords m_prevMoveVec = Movement<1>::NONE;

	char m_pushingTurn = 0;

	Data m_data;

	static constexpr char turnsNeededToPush = 5;

	static std::vector<const Photos::PreloadedAnimation*> m_animationsList;
};

class Shadow final : public TemporaryEntity
{
public:
	Shadow(const Coords& entityCoords, SmoothlyMovableEntity* const entityShadowOf);

	SmoothlyMovableEntity* const shadowOf;

	virtual ~Shadow() override;
};

class WallEntity final : public TexturedEntity
{
public:
	WallEntity(const Coords& entityCoords);
};

class BushEntity final : public TexturedEntity
{
public:
	BushEntity(const Coords& entityCoords);
};

class BushParticlesEntity final : public TemporaryAnimatedEntity
{
public:
	BushParticlesEntity(const Coords& entityCoords);

	static void resetStaticResources();

private:
	static std::vector<const Photos::PreloadedAnimation*> m_animationsList;
};

class WallWayEntity final : public TexturedEntity
{
public:
	WallWayEntity(const Coords& entityCoords);
};

class WallHiddenWayEntity final : public TexturedEntity
{
public:
	WallHiddenWayEntity(const Coords& entityCoords);
};

class RockEntity final : public FallingRotatableEntity, public TexturedEntity
{
public:
	RockEntity(const Coords& entityCoords);

protected:
	virtual void calcUpdateState() override;

	int m_holdingTurn = 0;
};

class DiamondEntity final : public FallingRotatableEntity, public TexturedEntity
{
public:
	DiamondEntity(const Coords& entityCoords);

protected:
	virtual void calcUpdateState() override;
};

class DiamondParticlesEntity final : public TemporaryAnimatedEntity
{
public:
	DiamondParticlesEntity(const Coords& entityCoords);

	static void resetStaticResources();

private:
	static std::vector<const Photos::PreloadedAnimation*> m_animationsList;
};

using EntitiesClassesList = std::tuple<PlayerEntity, Shadow, WallEntity, BushEntity, BushParticlesEntity, WallWayEntity, WallHiddenWayEntity, RockEntity, DiamondEntity, DiamondParticlesEntity>;