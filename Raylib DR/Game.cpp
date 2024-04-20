#include "Game.h"

#include <functional>

#include "Entities.h"
#include "options.h"
#include "photos_data.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

Game::Game(const std::string& windowTitle, int level) :
    m_currentLevel{ level }
{
    static_assert(Options::MovesPerSecond < Options::FPS);

	this->init(windowTitle, level);
}

void Game::init(const std::string& windowTitle, int level)
{
    InitWindow(Options::WorldSize.x + Options::SidebarWidth, Options::WorldSize.y, windowTitle.c_str());
    m_eventsHandler = EventsHandler({ Options::SidebarWidth, 0 }, Options::WorldSize);
    m_photos = LevelsPhotos[0];
    m_menu = std::make_unique<Menu>(m_photos, m_eventsHandler, Coords{ Options::WorldSize.x + Options::SidebarWidth, Options::WorldSize.y });

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(MainloopCallback, (void*)this, Options::FPS, true);
#else
    SetTargetFPS(Options::FPS);

    while (!WindowShouldClose() && !m_shouldExit)
    {
        this->mainloop();
    }

    CloseWindow();
#endif
}

void Game::createWorld()
{
    m_photos = LevelsPhotos[m_currentLevel];
    m_menu->rebindPhotos(m_photos);
    m_world = std::make_unique<World>(
        m_photos,
        m_eventsHandler,
        m_playerData,
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
    m_eventsHandler.update();

    if (!m_inMenu)
    {
        m_eventsHandler.handleEvents();

        if (m_world->player->getData().health == 0 && m_eventsHandler.enterEventSource)
        {
            m_world.reset();
            m_menu->setState(Menu::State::MENU);
            m_inMenu = true;
        }
        else if (m_world->player->getData().health == 0 && m_eventsHandler.enterEventSource)
        {
            m_world.reset();
            m_menu->setState(Menu::State::MENU);
            m_inMenu = true;
        }
        else if (m_eventsHandler.pauseEventSource)
        {
            m_menu->setPlayerData(m_world->player->getData());
            m_menu->setState(Menu::State::PAUSE);
            m_inMenu = true;
        }
        else if (m_world->currentFrame == 0)
        {
            m_world->update();
        }
    }

    BeginDrawing();
    ClearBackground(BLACK);

    if (m_inMenu)
    {
        switch (m_menu->draw())
        {
        case Menu::Signal::EXIT:
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
            CloseWindow();
            return;
#endif
            m_shouldExit = true;
            break;

        case Menu::Signal::EXIT_TO_MENU:
            m_world.reset();
            m_menu->setPlayerData(m_playerData);
            m_menu->setState(Menu::State::MENU);
            break;

        case Menu::Signal::NEW_GAME:
            m_currentLevel = 1;
            m_playerData = {};
            this->createWorld();
            m_inMenu = false;
            break;

        case Menu::Signal::CONTINUE:
            m_inMenu = false;
            break;

        case Menu::Signal::SAVE:
            m_world->saveCheckpoint();
            break;

        case Menu::Signal::LOAD:
            m_world->loadCheckpoint();
            m_menu->setPlayerData(m_world->player->getData());
            break;

        case Menu::Signal::LAST_LEVEL:
            m_world.reset();
            this->createWorld();
            m_inMenu = false;
            break;
        }
    }
    else
    {
        m_world->draw();
    }

    EndDrawing();
}

#ifdef __EMSCRIPTEN__
void MainloopCallback(void* arg)
{
    static_cast<Game*>(arg)->mainloop();
}
#endif
