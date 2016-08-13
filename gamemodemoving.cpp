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
	const int factor = this->app()->clipPackage()->rounds() * 2 - this->app()->turns();

	return 2000 * factor + 6000;
}

void GameModeMoving::afterNarrator()
{
	this->m_roomWidget->floatingClips().at(0)->setClip(m_currentSolution);

	// add more floating clips every round to make it a bit harder
	if (this->app()->turns() > 1)
	{
		const int index = qrand() % this->app()->completeSolution().size();

		// make them faster than the  first clip
		const int speed = FLOATING_CLIP_PIXELS_PER_S + this->app()->turns() * 40;
		qDebug() << "Speed: " << speed;
		this->m_roomWidget->addFloatingClip(this->app()->completeSolution().at(index), this->m_roomWidget->floatingClips().at(0)->width(), speed);

		foreach (FloatingClip *clip, this->m_roomWidget->floatingClips())
		{
			clip->setSpeed(speed);
		}
	}

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

		// When no more clips are remaining, the game has been won.
		if (copy.isEmpty())
		{
			// Disable click events and timers
			this->pause();
			setState(State::Won);

			return;
		}

		// Select a random clip.
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
	// dont delete the room widget, which leads to segmentation fault, since the end() method is called from the slot of the signal gotIt()
	this->m_roomWidget->hide();

	this->m_currentSolution = nullptr;
	this->m_remainingClips.clear();
	setState(State::None);
}

void GameModeMoving::start()
{
	// the room widget is cached
	if (this->m_roomWidget == nullptr)
	{
		this->m_roomWidget = new RoomWidget(this, this->app()->centralWidget());
		connect(this->m_roomWidget, SIGNAL(gotIt()), this, SLOT(gotIt()));
		this->app()->gameAreaLayout()->addWidget(this->m_roomWidget);
	}
	else
	{
		this->m_roomWidget->clearFloatingClipsExceptFirst();
		this->m_roomWidget->show();
	}

	this->m_roomWidget->setEnabled(false);

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
