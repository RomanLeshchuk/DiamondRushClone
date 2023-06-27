#include "Sidebar.h"

#include "raylib.h"

#include "World.h"

Sidebar::Sidebar(World* world) :
	m_size{ world->sidebarWidth, world->viewportSize.y * world->cellSize.y }, m_texture{ world->photos.getSimpleTexture("sidebar") }
{
}

void Sidebar::draw()
{
	/*DrawTexturePro(
		*m_texture,
		{ 0.0f, 0.0f, (float)m_texture->width, (float)m_texture->height },
		{ 0.0f, 0.0f, (float)m_size.x, (float)m_size.y },
		{ 0.0f, 0.0f },
		0.0f,
		WHITE
	);*/
}