#pragma once

#include "raylib.h"

#include <string>

#include "data_types.h"
#include "EventsHandler.h"
#include "Menu.h"
#include "World.h"

class Game
{
public:
    Game(const std::string& windowTitle, int level);

	void mainloop();

private:
	void init(const std::string& windowTitle, int level);
	void createWorld(int level);

	std::unique_ptr<World> m_world = nullptr;
	std::unique_ptr<Menu> m_menu = nullptr;
	bool m_inMenu = true;
	bool m_shouldExit = false;

	int m_currentLevel;
	Photos m_photos{};
	EventsHandler m_eventsHandler{};
};

#ifdef __EMSCRIPTEN__
void MainloopCallback(void* arg);
#endif