#include "gamemodemoving.h"
#include "roomwidget.h"
#include "fairytale.h"
#include "clippackage.h"
#include "floatingclip.h"
#include "clip.h"

GameModeMoving::GameModeMoving(fairytale* app): GameMode(app), m_state(State::None), m_currentSolution(nullptr), m_roomWidget(nullptr)
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
	return 2000 * (this->m_remainingClips.size() + 1);
}

void GameModeMoving::afterNarrator()
{
	this->m_roomWidget->floatingClip()->setClip(m_currentSolution);
	this->m_roomWidget->start();
}

void GameModeMoving::nextTurn()
{
	this->m_currentSolution = nullptr;

	if (!this->m_remainingClips.empty())
	{
		QList<Clip*> copy = this->m_remainingClips;

		/*
		 * Allow only persons or acts.
		 */
		for (int i = 0; i < copy.size(); )
		{
			if ((app()->requiresPerson() && !copy[i]->isPerson()) || (!app()->requiresPerson() && copy[i]->isPerson()))
			{
				copy.removeAt(i);
			}
			else
			{
				++i;
			}
		}

		if (copy.isEmpty())
		{
			// Disable click events and timers
			this->pause();
			setState(State::Won);

			return;
		}

		const int index = qrand() % copy.size();
		m_currentSolution = copy.at(index);
		m_remainingClips.removeAll(m_currentSolution);

		setState(State::Running);
	}
	else
	{
		// Disable click events and timers
		this->pause();
		setState(State::Won);
	}
}

void GameModeMoving::resume()
{
	if (this->m_roomWidget != nullptr)
	{
		this->m_roomWidget->resume();
	}
}

void GameModeMoving::pause()
{
	if (this->m_roomWidget != nullptr)
	{
		this->m_roomWidget->pause();
	}
}

void GameModeMoving::end()
{
	// Make sure the timers stop
	this->m_roomWidget->pause();
	delete this->m_roomWidget;
	this->m_roomWidget = nullptr;

	this->m_currentSolution = nullptr;
	this->m_remainingClips.clear();
	setState(State::None);
}

void GameModeMoving::start()
{
	this->m_roomWidget = new RoomWidget(this->app());
	connect(this->m_roomWidget, SIGNAL(gotIt()), this, SLOT(gotIt()));
	this->app()->gameAreaLayout()->addWidget(this->m_roomWidget);

	this->m_remainingClips.clear();

	foreach (Clip *clip, app()->clipPackage()->clips())
	{
		this->m_remainingClips.push_back(clip);
	}
}

void GameModeMoving::gotIt()
{
	this->m_roomWidget->pause();
	this->app()->onFinishTurn();
}

void GameModeMoving::lost()
{
	// Disable click events and timers
	this->pause();
	this->setState(State::Lost);

	this->app()->onFinishTurn();
}

#include "gamemodemoving.moc"
