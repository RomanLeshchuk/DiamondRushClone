#include "Game.h"

#include <functional>

#include "Entities.h"
#include "options.h"
#include "photos_data.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

Game::Game(const std::string& windowTitle, int level)
{
    static_assert(Options::MovesPerSecond < Options::FPS);

	this->init(windowTitle, level);
}

void Game::init(const std::string& windowTitle, int level)
{
    InitWindow(Options::WorldSize.x + Options::SidebarWidth, Options::WorldSize.y, windowTitle.c_str());

    this->createWorld(level);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(MainloopCallback, (void*)this, Options::FPS, true);
#else
    SetTargetFPS(Options::FPS);

    while (!WindowShouldClose())
    {
        this->mainloop();
    }
#endif

    CloseWindow();
}

void Game::createWorld(int level)
{
    world.reset();
    world = std::make_unique<World>(
        LevelsPhotos[level],
        &m_eventsHandler,
        Options::ViewportSize,
        Options::UpdateRectSize,
        Options::WorldSize,
        Options::SidebarWidth,
        Options::FramesPerMove,
        Options::MaxPlayerShift
    );
}

void Game::mainloop()
{
    m_eventsHandler.handleEvents();

    if (!(*world->player->getHealth()) && m_eventsHandler.enterEventSource)
    {
        this->createWorld(m_currentLevel);
    }

    if (world->currentFrame == 0)
    {
        world->update();
    }

    BeginDrawing();
    ClearBackground(BLACK);
    world->draw();

    EndDrawing();
}

#ifdef __EMSCRIPTEN__
void MainloopCallback(void* arg)
{
    static_cast<Game*>(arg)->mainloop();
}
#endif
