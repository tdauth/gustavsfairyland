#include "gamemode.h"
#include "fairytale.h"

GameMode::GameMode(fairytale *app) : QObject(app), m_app(app)
{
}

GameMode::~GameMode()
{
}