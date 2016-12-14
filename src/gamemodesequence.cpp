#include "gamemodesequence.h"
#include "roomwidget.h"
#include "fairytale.h"
#include "clippackage.h"
#include "floatingclip.h"
#include "clip.h"
#include "solutionwidget.h"

GameModeSequence::GameModeSequence(fairytale* app): GameMode(app), m_state(State::None), m_roomWidget(nullptr), m_solutionWidget(nullptr)
{
}

GameMode::State GameModeSequence::state()
{
	return this->m_state;
}

GameMode::ClipKeys GameModeSequence::solutions()
{
	return m_currentSolution;
}

long int GameModeSequence::time()
{
	int startValue = 8000;
	int perClipValue = 1500;

	switch (app()->difficulty())
	{
		case fairytale::Difficulty::Easy:
		{
			startValue = 10000;
			perClipValue = 1600;

			break;
		}

		case fairytale::Difficulty::Normal:
		{
			startValue = 8000;
			perClipValue = 1500;

			break;
		}

		case fairytale::Difficulty::Hard:
		{
			startValue = 6000;
			perClipValue = 1400;

			break;
		}

		case fairytale::Difficulty::Mahlerisch:
		{
			startValue = 4000;
			perClipValue = 1300;

			break;
		}
	}

	// More time to order all floating clips!
	return (perClipValue * maxFloatingClips + startValue);
}

void GameModeSequence::afterNarrator()
{
	this->m_roomWidget->clearFloatingClips();

	for (int i = 0; i < m_currentSolution.size(); ++i)
	{
		this->m_roomWidget->addFloatingClip(m_currentSolution[i], this->m_roomWidget->floatingClipWidth(), this->m_roomWidget->floatingClipSpeed());
		this->m_solutionWidget->addClip(m_currentSolution[i]);
	}

	this->m_roomWidget->clearClickAnimations();

	this->m_roomWidget->start();
}

void GameModeSequence::nextTurn()
{
	m_currentSolution.clear();
	m_solutionWidget->clearClips();

	if (!this->m_remainingClips.empty())
	{
		bool requiresPerson = true;

		// Select a random clips from remaining.
		for (int i = 0; i < maxFloatingClips; ++i)
		{
			QList<fairytale::ClipKey> copy = this->m_remainingClips;

			/*
			* Allow only persons or acts.
			*/
			for (int j = 0; j < copy.size(); )
			{
				Clip *clip = app()->getClipByKey(copy[j]);

				if ((requiresPerson && !clip->isPerson()) || (!requiresPerson && clip->isPerson()))
				{
					copy.removeAt(j);
				}
				else
				{
					++j;
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

			const int index = qrand() % copy.size();
			const fairytale::ClipKey clipKey = copy.at(index);
			m_currentSolution.push_back(clipKey);
			copy.removeAt(index);
			m_remainingClips.removeAll(clipKey);

			if (i >= 1)
			{
				requiresPerson = !requiresPerson;
			}
		}

		setState(State::Running);
	}
	else
	{
		// Disable click events and timers
		this->pause();
		setState(State::Won);
	}
}

void GameModeSequence::resume()
{
	if (this->m_roomWidget != nullptr)
	{
		this->m_roomWidget->resume();
	}
}

void GameModeSequence::pause()
{
	if (this->m_roomWidget != nullptr)
	{
		this->m_roomWidget->pause();
	}
}

void GameModeSequence::end()
{
	// Make sure the timers stop
	this->m_roomWidget->pause();
	// Dont delete the room widget, which leads to segmentation fault, since the end() method is called from the slot of the signal gotIt().
	this->m_roomWidget->hide();

	this->m_remainingClips.clear();

	// NOTE Never clear clips. This will lead to a segmentation fault if the drop event is not handled yet.
	m_solutionWidget->hide();

	// Dont clear the state for the custom fairy tale dialog
	//setState(State::None);
}

void GameModeSequence::start()
{
	// the room widget is cached
	if (this->m_roomWidget == nullptr)
	{
		this->m_roomWidget = new RoomWidget(this, RoomWidget::Mode::DragAndDrop, this->app()->gameAreaWidget());
		this->app()->gameAreaLayout()->addWidget(this->m_roomWidget);
		double factor = 0.30;

		switch (app()->difficulty())
		{
			case fairytale::Difficulty::Easy:
			{
				factor = 0.30;

				break;
			}

			case fairytale::Difficulty::Normal:
			{
				factor = 0.50;

				break;
			}

			case fairytale::Difficulty::Hard:
			{
				factor = 0.70;

				break;
			}

			case fairytale::Difficulty::Mahlerisch:
			{
				factor = 0.90;

				break;
			}
		}

		// Make floating clips slower for drag & drop which is harder than just clicking.
		this->m_roomWidget->setFloatingClipSpeedFactor(factor);
	}
	else
	{
		this->m_roomWidget->clearClickAnimations();
		this->m_roomWidget->show();
	}

	if (this->m_solutionWidget == nullptr)
	{
		this->m_solutionWidget = new SolutionWidget(this->app(), this->app()->gameAreaWidget());
		connect(this->m_solutionWidget, &SolutionWidget::solved, this, &GameModeSequence::solved);
		connect(this->m_solutionWidget, &SolutionWidget::failed, this, &GameModeSequence::failed);
		this->app()->gameAreaLayout()->addWidget(this->m_solutionWidget);
	}

	m_solutionWidget->show();

	this->m_roomWidget->setEnabled(false);
	this->m_roomWidget->setSolutionWidget(this->m_solutionWidget);

	this->m_remainingClips.clear();

	for (QString packageId : app()->currentClipPackages().keys())
	{
		ClipPackage *clipPackage = app()->getClipPackageById(packageId);

		for (Clip *clip : clipPackage->clips())
		{
			this->m_remainingClips.push_back(fairytale::ClipKey(packageId, clip->id()));
		}
	}
}

void GameModeSequence::solved(const fairytale::ClipKey &clipKey)
{
	qDebug() << "Succeed floating clip";
	this->m_roomWidget->playSuccessSound();

	// Only pause and hide the floating clip. Otherwise we are still in a function of the corresponding floating clip and cant delete it.
	this->m_roomWidget->hideFloatingClip(clipKey);

	if (m_solutionWidget->solvedAll())
	{
		// Disable click events and timers
		this->pause();
		setState(State::Won);

		// Has to be added manually since the state is already won.
		this->app()->addCurrentSolution();
		this->app()->onFinishTurn();
	}
}

void GameModeSequence::failed(const fairytale::ClipKey &clipKey)
{
	// TODO reset floating clip
	qDebug() << "Failed floating clip";
	this->m_roomWidget->playFailSound();
}

void GameModeSequence::lost()
{
	this->m_roomWidget->cancelDrags();

	// Handle drop event before finishing turn.
	QEventLoop eventLoop(m_solutionWidget);
	eventLoop.processEvents(QEventLoop::AllEvents);
	eventLoop.quit();

	// Disable click events and timers
	this->pause();
	this->setState(State::Lost);

	this->app()->onFinishTurn();
}

bool GameModeSequence::hasToChooseTheSolution()
{
	return false;
}

bool GameModeSequence::useMaxRounds()
{
	return false;
}

#include "gamemodesequence.moc"
