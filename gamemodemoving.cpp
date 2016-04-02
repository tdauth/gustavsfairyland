#include "gamemodemoving.h"
#include "roomwidget.h"
#include "fairytale.h"

GameModeMoving::GameModeMoving(fairytale* app): GameMode(app), m_state(State::None), m_roomWidget(nullptr)
{
}

GameMode::State GameModeMoving::state()
{
	return this->m_state;
}

Clip* GameModeMoving::solution()
{
	return this->m_currentSolution;
}

long int GameModeMoving::time()
{
	return 10000;
}

void GameModeMoving::afterNarrator()
{

}

void GameModeMoving::nextTurn()
{

}

void GameModeMoving::resume()
{

}

void GameModeMoving::pause()
{

}

void GameModeMoving::end()
{

}

void GameModeMoving::start()
{
	this->m_roomWidget = new RoomWidget(this->app());
	this->app()->gameAreaLayout()->addWidget(this->m_roomWidget);
}
