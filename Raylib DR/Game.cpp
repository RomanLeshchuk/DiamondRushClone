#include "Game.h"

#include <functional>

#include "options.h"
#include "photos_data.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

Game::Game(const std::string_view& windowTitle, int level)
{
	this->init(windowTitle, level);
}

void Game::init(const std::string_view& windowTitle, int level)
{
    InitWindow(Options::WindowSize.x, Options::WindowSize.y, windowTitle.data());

    world = std::make_unique<World>(LevelsPhotos[level], &m_eventsHandler, Options::ViewportSize, Options::UpdateRectSize, Options::WindowSize, Options::FramesPerMove, Options::MaxPlayerShift);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(MainloopCallback, (void*)this, 0, true);
#else
    SetTargetFPS(Options::FPS);

    while (!WindowShouldClose())
    {
        this->mainloop();
    }
#endif

    CloseWindow();
}

void Game::mainloop()
{
    m_eventsHandler.handleEvents();

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