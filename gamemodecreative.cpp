#include "gamemodecreative.h"
#include "fairytale.h"
#include "clippackage.h"
#include "iconbutton.h"
#include "clip.h"

void GameModeCreative::clickCard()
{
	bool success = false;

	for (int i = 0; i < this->m_buttons.size() && !success; ++i)
	{
		if (QObject::sender() == this->m_buttons[i]->iconButton())
		{
			this->m_currentSolution = this->m_buttons[i]->clip();
			success = true;
		}
	}

	this->app()->onFinishTurn();
}

void GameModeCreative::finish()
{
	this->setState(State::Won);

	this->app()->onFinishTurn();
}


GameModeCreative::GameModeCreative(fairytale *app) : GameMode(app), m_state(State::None), m_currentSolution(nullptr), m_finishButton(nullptr)
{
}

GameMode::State GameModeCreative::state()
{
	return this->m_state;
}

Clip* GameModeCreative::solution()
{
	return this->m_currentSolution;
}

long int GameModeCreative::time()
{
	return 0;
}

void GameModeCreative::afterNarrator()
{
	int row = 0;
	int column = 0;
	int counter = 0;

	ClipPackage *clipPackage = this->app()->clipPackage();
	ClipPackage::Clips::iterator iterator = clipPackage->clips().begin();

	for (int i = 0; i < clipPackage->clips().size() && iterator != clipPackage->clips().end(); ++i, ++iterator)
	{
		Clip *clip = *iterator;

		// never use a clip twice and only show expected clips
		if (this->app()->completeSolution().contains(clip) || (this->app()->requiresPerson() && !clip->isPerson()) || (!this->app()->requiresPerson() && clip->isPerson()))
		{
			continue;
		}

		ClipButton *button = new ClipButton(this->app(), clip);

		this->m_buttons.push_back(button);
		this->app()->gameAreaLayout()->addWidget(button, row, column);
		button->show(); // first show and resize
		button->updateGeometry();

		if ((counter + 1) % 4 == 0)
		{
			++row;
			column = 0;
		}
		else
		{
			column++;
		}

		++counter;
	}

	m_finishButton = new QPushButton(this->app());
	m_finishButton->setText(tr("Complete"));
	this->app()->gameAreaLayout()->addWidget(m_finishButton, row, column);
	connect(m_finishButton, &QPushButton::clicked, this, &GameModeCreative::finish);

	// now show buttons
	for (int i = 0; i < m_buttons.size(); ++i)
	{
		ClipButton *button = m_buttons[i];
		Clip *clip = button->clip();
		const QUrl url = this->app()->resolveClipUrl(clip->imageUrl());
#ifndef Q_OS_ANDROID
		const QString filePath = url.toLocalFile();
#else
		const QString filePath = url.url();
#endif

		button->iconButton()->setFile(filePath);
		connect(button->iconButton(), &IconButton::clicked, this, &GameModeCreative::clickCard);
		button->label()->setText(this->app()->description(this->app()->startPerson(), this->app()->turns(), clip, false));
		button->show();
	}

	// TODO this->app()->onFinishTurn(); when solution is clicked
}

void GameModeCreative::nextTurn()
{
	this->m_currentSolution = nullptr;

	foreach (ClipButton *button, this->m_buttons)
	{
		delete button;
	}

	this->m_buttons.clear();

	if (m_finishButton != nullptr)
	{
		delete m_finishButton;
		m_finishButton = nullptr;
	}

	setState(State::Running);
}

void GameModeCreative::resume()
{
	foreach (ClipButton *button, this->m_buttons)
	{
		button->setEnabled(true);
	}
}

void GameModeCreative::pause()
{
	foreach (ClipButton *button, this->m_buttons)
	{
		button->setEnabled(false);
	}
}

void GameModeCreative::end()
{
	this->m_currentSolution = nullptr;

	foreach (ClipButton *button, this->m_buttons)
	{
		delete button;
	}

	this->m_buttons.clear();

	if (m_finishButton != nullptr)
	{
		delete m_finishButton;
		m_finishButton = nullptr;
	}
}

void GameModeCreative::start()
{
}

QString GameModeCreative::name() const
{
	return tr("Creative Mode");
}

QString GameModeCreative::id() const
{
	return "creative";
}

void GameModeCreative::setState(State state)
{
	this->m_state = state;
}

bool GameModeCreative::hasToChooseTheSolution()
{
	return false;
}

bool GameModeCreative::hasLimitedTime()
{
	return false;
}

bool GameModeCreative::playIntro()
{
	return false;
}

bool GameModeCreative::playOutro()
{
	return false;
}

bool GameModeCreative::showWinDialog()
{
	return false;
}

bool GameModeCreative::unlockBonusClip()
{
	return false;
}

bool GameModeCreative::addToHighScores()
{
	return false;
}

GameModeCreative::ClipButton::ClipButton(QWidget *parent, Clip *clip) : QWidget(parent), m_clip(clip), m_iconButton(new IconButton(this)), m_label(new QLabel(this))
{
	QVBoxLayout *layout = new QVBoxLayout();
	setLayout(layout);
	layout->addWidget(m_iconButton);
	layout->addWidget(m_label);
	layout->setAlignment(m_label, Qt::AlignCenter);
}

