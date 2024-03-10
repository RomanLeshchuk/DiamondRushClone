#pragma once

#include "data_types.h"
#include "Photos.h"
#include "Text.h"

class World;
class PlayerEntity;

class Sidebar
{
public:
	Sidebar() = default;
	Sidebar(World* world);

	void draw();

private:
	PlayerEntity* m_player;
	Coords m_size;
	const Photos::PreloadedSimpleTexture* m_texture;
	std::vector<Text> m_texts{};
	std::vector<Counter> m_counters{};
};