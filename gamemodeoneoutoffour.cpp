#include "gamemodeoneoutoffour.h"
#include "fairytale.h"
#include "clip.h"
#include "clippackage.h"
#include "iconbutton.h"

GameModeOneOutOfFour::GameModeOneOutOfFour(fairytale *app, int size) : GameMode(app), m_size(size), m_state(State::None)
{
}

void GameModeOneOutOfFour::clickCard()
{
	bool success = false;

	for (int i = 0; i < this->m_currentClips.size(); ++i)
	{
		if (QObject::sender() == this->m_buttons[i] && this->m_currentSolution == this->m_currentClips[i])
		{
			success = true;
		}

		this->m_buttons[i]->setIcon(QIcon());
		this->m_buttons[i]->setEnabled(false);
	}

	if (!success)
	{
		this->setState(State::Lost);
	}

	this->app()->onFinishTurn();
}

void GameModeOneOutOfFour::start()
{

	for (int i = 0; i < this->m_buttons.size(); ++i)
	{
		delete this->m_buttons[i];
	}

	this->m_buttons.clear();

	this->m_remainingClips.clear();

	foreach (Clip *clip, app()->clipPackage()->clips())
	{
		this->m_remainingClips.push_back(clip);
	}

	int row = 0;
	int column = 0;

	for (int i = 0; i < this->size(); ++i)
	{
		IconButton *button = new IconButton(this->app());
		this->m_buttons.push_back(button);
		this->app()->gameAreaLayout()->addWidget(button, row, column);
		connect(button, SIGNAL(clicked()), this, SLOT(clickCard()));
		button->hide();

		if ((i + 1) % 2 == 0)
		{
			++row;
			column = 0;
		}
		else
		{
			column++;
		}
	}
}

void GameModeOneOutOfFour::end()
{
	// dont delete buttons since their signal is connected to clickCard() from which this method might indirectly be called
	for (int i = 0; i < this->size(); ++i)
	{
		this->m_buttons[i]->hide();
	}

	this->m_currentClips.clear();
	this->m_currentSolution = nullptr;
	this->m_remainingClips.clear();
	setState(State::None);
}

void GameModeOneOutOfFour::resume()
{
	foreach (QPushButton *button, this->m_buttons)
	{
		button->setEnabled(true);
	}
}

void GameModeOneOutOfFour::pause()
{
	foreach (QPushButton *button, this->m_buttons)
	{
		button->setEnabled(false);
	}
}

GameMode::State GameModeOneOutOfFour::state()
{
	return m_state;
}

Clip* GameModeOneOutOfFour::solution()
{
	return this->m_currentSolution;
}

void GameModeOneOutOfFour::nextTurn()
{
	this->m_currentClips.clear();
	this->m_currentSolution = nullptr;

	if (!this->m_remainingClips.empty())
	{
		for (int i = 0; i < this->m_currentClips.size(); ++i)
		{
			this->m_buttons[i]->setIcon(QIcon());
			this->m_buttons[i]->setEnabled(false);
		}

		this->fillCurrentClips();

		if (!this->m_currentClips.empty())
		{
			this->selectRandomSolution();
			setState(State::Running);
		}
		else
		{
			setState(State::Won);
		}
	}
	else
	{
		setState(State::Won);
	}
}

void GameModeOneOutOfFour::afterNarrator()
{
	for (int i = 0; i < this->m_currentClips.size(); ++i)
	{
		this->m_buttons[i]->show();
		this->m_buttons[i]->setFile(this->app()->resolveClipUrl(this->m_currentClips[i]->imageUrl()).toLocalFile());
		this->m_buttons[i]->setEnabled(true);
	}

	for (int i = this->m_currentClips.size(); i < this->size(); ++i)
	{
		this->m_buttons[i]->setFile(QString());
		this->m_buttons[i]->setEnabled(false);
		this->m_buttons[i]->hide();
	}
}

long int GameModeOneOutOfFour::time()
{
	return 2000 * (this->m_remainingClips.size() + 1);
}

void GameModeOneOutOfFour::fillCurrentClips()
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
		return;
	}

	qDebug() << "Size before" << copy.size();

	for (int i = 0; i < this->m_buttons.size() && !copy.empty(); ++i)
	{
		const int index = qrand() % copy.size();
		this->m_currentClips.push_back(copy[index]);
		copy.removeAt(index);
		//qDebug() << "Index: " << index << " and size " << copy.size();
	}

	qDebug() << "Current size of buttons " << this->m_currentClips.size();
}

void GameModeOneOutOfFour::selectRandomSolution()
{
	const int index = qrand() % this->m_currentClips.size();
	Clip *solution = this->m_currentClips[index];
	this->m_currentSolution = solution;
	this->m_remainingClips.removeAll(solution); // solution is done forever
}

