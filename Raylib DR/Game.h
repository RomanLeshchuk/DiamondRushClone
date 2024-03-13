#pragma once

#include "raylib.h"

#include <string>
#include <limits>

#include "data_types.h"
#include "EventsHandler.h"
#include "World.h"

class Game
{
public:
    Game(const std::string& windowTitle, int level);

	void mainloop();

private:
	void init(const std::string& windowTitle, int level);
	void createWorld(int level);

	std::unique_ptr<World> world = nullptr;
	int m_currentLevel = 0;
	EventsHandler m_eventsHandler{};
};

#ifdef __EMSCRIPTEN__
void MainloopCallback(void* arg);
#endif