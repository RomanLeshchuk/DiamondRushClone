#pragma once

#include "raylib.h"

#include <string_view>
#include <limits>

#include "data_types.h"
#include "EventsHandler.h"
#include "World.h"

class Game
{
public:
    Game(const std::string_view& windowTitle, int level);

	void mainloop();

private:
	void init(const std::string_view& windowTitle, int level);

	std::unique_ptr<World> world = nullptr;
	EventsHandler m_eventsHandler{};
};

#ifdef __EMSCRIPTEN__
void MainloopCallback(void* arg);
#endif