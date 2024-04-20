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
		int level = 1;
	};

	PlayerEntity(const Coords& entityCoords, const Coords* moveEventSource, const Data& playerData);

	void changeDiamonds(int value);
	void changeHealth(int value);

	const Data& getData();

	static void resetStaticResources();

protected:
	virtual PlayerEntity* copyImpl() const override;

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

	SmoothlyMovableEntity* shadowOf;

	virtual ~Shadow() override;

protected:
	virtual Shadow* copyImpl() const override;
};

class WallEntity final : public TexturedEntity
{
public:
	WallEntity(const Coords& entityCoords);

protected:
	virtual WallEntity* copyImpl() const override;
};

class BushEntity final : public TexturedEntity
{
public:
	BushEntity(const Coords& entityCoords);

protected:
	virtual BushEntity* copyImpl() const override;
};

class BushParticlesEntity final : public TemporaryAnimatedEntity
{
public:
	BushParticlesEntity(const Coords& entityCoords);

	static void resetStaticResources();

protected:
	virtual BushParticlesEntity* copyImpl() const override;

private:
	static std::vector<const Photos::PreloadedAnimation*> m_animationsList;
};

class WallWayEntity final : public TexturedEntity
{
public:
	WallWayEntity(const Coords& entityCoords);

protected:
	virtual WallWayEntity* copyImpl() const override;
};

class WallHiddenWayEntity final : public TexturedEntity
{
public:
	WallHiddenWayEntity(const Coords& entityCoords);

protected:
	virtual WallHiddenWayEntity* copyImpl() const override;
};

class RockEntity final : public FallingRotatableEntity, public TexturedEntity
{
public:
	RockEntity(const Coords& entityCoords);

protected:
	virtual RockEntity* copyImpl() const override;

	virtual void calcUpdateState() override;

	int m_holdingTurn = 0;
};

class DiamondEntity final : public FallingRotatableEntity, public TexturedEntity
{
public:
	DiamondEntity(const Coords& entityCoords);

protected:
	virtual DiamondEntity* copyImpl() const override;

	virtual void calcUpdateState() override;
};

class DiamondParticlesEntity final : public TemporaryAnimatedEntity
{
public:
	DiamondParticlesEntity(const Coords& entityCoords);

	static void resetStaticResources();

protected:
	virtual DiamondParticlesEntity* copyImpl() const override;

private:
	static std::vector<const Photos::PreloadedAnimation*> m_animationsList;
};

class FinishEntity final : public TexturedEntity
{
public:
	FinishEntity(const Coords& entityCoords);

protected:
	virtual FinishEntity* copyImpl() const override;
};

using EntitiesClassesList = std::tuple<PlayerEntity, Shadow, WallEntity, BushEntity, BushParticlesEntity, WallWayEntity, WallHiddenWayEntity, RockEntity, DiamondEntity, DiamondParticlesEntity, FinishEntity>;