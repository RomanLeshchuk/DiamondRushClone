#include "Sidebar.h"

#include "raylib.h"

#include "World.h"
#include "Entities.h"

Sidebar::Sidebar(World* world) :
	m_size{ world->sidebarWidth, (2 * world->viewportSize.y + 1) * world->cellSize.y }, m_texture{ world->photos->getSimpleTexture("sidebar") }, m_player{ world->player }
{
	const int defaultFontSize = m_size.y / 22;
	m_texts.emplace_back("Progress", Coords{ m_size.x / 2, (int)(m_size.y / 15.0f) }, defaultFontSize, BLACK);
	m_counters.emplace_back("Health", &m_player->getData().health, Coords{ m_size.x / 2, (int)(m_size.y / 2.0f) - defaultFontSize }, defaultFontSize, BLACK);
	m_counters.emplace_back("Diamonds", &m_player->getData().diamondsCollected, Coords{ m_size.x / 2, (int)(m_size.y / 2.0f) }, defaultFontSize, BLACK);
	m_counters.emplace_back("Level", &m_player->getData().level, Coords{ m_size.x / 2, (int)(m_size.y / 2.0f) + defaultFontSize }, defaultFontSize, BLACK);
}

void Sidebar::draw()
{
	DrawTexturePro(
		*m_texture,
		{ 0.0f, 0.0f, (float)m_texture->width, (float)m_texture->height },
		{ 0.0f, 0.0f, (float)m_size.x, (float)m_size.y },
		{ 0.0f, 0.0f },
		0.0f,
		WHITE
	);

	for (const Text& text : m_texts)
	{
		text.draw();
	}

	for (const Counter& counter : m_counters)
	{
		counter.draw();
	}
}