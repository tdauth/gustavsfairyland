#include "gamemodemoving.h"
#include "roomwidget.h"
#include "fairytale.h"
#include "clippackage.h"
#include "floatingclip.h"
#include "clip.h"

GameModeMoving::GameModeMoving(fairytale* app): GameMode(app), m_state(State::None), m_roomWidget(nullptr)
{
}

GameMode::State GameModeMoving::state()
{
	return this->m_state;
}

GameMode::ClipKeys GameModeMoving::solutions()
{
	ClipKeys result;
	result.push_back(this->m_currentSolution);

	return result;
}

long int GameModeMoving::time()
{
	int startValue = 8000;

	switch (app()->difficulty())
	{
		case fairytale::Difficulty::Easy:
		{
			startValue = 10000;

			break;
		}

		case fairytale::Difficulty::Normal:
		{
			startValue = 8000;

			break;
		}

		case fairytale::Difficulty::Hard:
		{
			startValue = 6000;

			break;
		}

		case fairytale::Difficulty::Mahlerisch:
		{
			startValue = 4000;

			break;
		}
	}

	// TODO calculate rounds by ALL used clip packages
	const int maxRoundsByClipPackages = this->app()->maxRoundsByMultipleClipPackages(this->app()->currentClipPackages());
	const int rounds = !app()->useMaxRounds() ?  maxRoundsByClipPackages : qMin(maxRoundsByClipPackages, app()->maxRounds());
	const int factor = rounds * 2 - this->app()->turns();

	return 1000 * factor + startValue;
}

void GameModeMoving::afterNarrator()
{
	this->m_roomWidget->addFloatingClip(m_currentSolution, this->m_roomWidget->floatingClipWidth(), this->m_roomWidget->floatingClipSpeed());
	this->m_roomWidget->clearClickAnimations();

	// add more floating clips every round to make it a bit harder
	if (this->app()->turns() > 0)
	{
		const int index = qrand() % this->app()->completeSolution().size();

		// Don't add too many floating clips, otherwise it becomes too easy since they're blocking each other all the time.
		if (this->m_roomWidget->floatingClips().size() < 6)
		{
			this->m_roomWidget->addFloatingClip(this->app()->completeSolution().at(index), this->m_roomWidget->floatingClipWidth(), this->m_roomWidget->floatingClipSpeed());
		}
	}

	qDebug() << "Starting room widget is shown" << this->m_roomWidget->isVisible();
	this->m_roomWidget->show();
	qDebug() << "Starting room widget is shown 2" << this->m_roomWidget->isVisible();
	qDebug() << "Gamea Area visible" << this->app()->gameAreaWidget()->isVisible();
	this->m_roomWidget->start();
}

void GameModeMoving::nextTurn()
{
	if (!this->m_remainingClips.empty() && (!app()->useMaxRounds() || app()->rounds() < app()->maxRounds()))
	{
		QList<fairytale::ClipKey> copy = this->m_remainingClips;

		/*
		 * Allow only persons or acts.
		 */
		for (int i = 0; i < copy.size(); )
		{
			Clip *clip = app()->getClipByKey(copy[i]);

			if ((app()->requiresPerson() && !clip->isPerson()) || (!app()->requiresPerson() && clip->isPerson()))
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

		// TEST
		this->m_roomWidget->show();

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
	// Dont delete the room widget, which leads to segmentation fault, since the end() method is called from the slot of the signal gotIt().
	this->m_roomWidget->hide();

	this->m_remainingClips.clear();

	// Dont clear the state for the custom fairy tale dialog
	//setState(State::None);
}

void GameModeMoving::start()
{
	// the room widget is cached
	if (this->m_roomWidget == nullptr)
	{
		this->m_roomWidget = new RoomWidget(this, RoomWidget::Mode::Click, this->app()->gameAreaWidget());
		connect(this->m_roomWidget, &RoomWidget::gotIt, this, &GameModeMoving::gotIt);
		this->app()->gameAreaLayout()->addWidget(this->m_roomWidget);
	}
	else
	{
		this->m_roomWidget->clearFloatingClips();
		this->m_roomWidget->clearClickAnimations();
		this->m_roomWidget->show();
	}

	this->m_roomWidget->setEnabled(false);

	this->m_remainingClips.clear();

	foreach (QString packageId, app()->currentClipPackages().keys())
	{
		ClipPackage *clipPackage = app()->getClipPackageById(packageId);

		foreach (Clip *clip, clipPackage->clips())
		{
			this->m_remainingClips.push_back(fairytale::ClipKey(packageId, clip->id()));
		}
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
