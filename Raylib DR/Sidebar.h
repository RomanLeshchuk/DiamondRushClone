#pragma once

#include "data_types.h"
#include "Photos.h"

class World;

class Sidebar
{
public:
	Sidebar(World* world);

	void draw();

private:
	Coords m_size;
	const Photos::PreloadedSimpleTexture* m_texture;
};