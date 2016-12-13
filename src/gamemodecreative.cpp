#include <QtGui>
#include <QtWidgets>

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


GameModeCreative::GameModeCreative(fairytale *app) : GameMode(app), m_state(State::None), m_finishButton(nullptr)
{
}

GameMode::State GameModeCreative::state()
{
	return this->m_state;
}

GameMode::ClipKeys GameModeCreative::solutions()
{
	ClipKeys result;
	result.push_back(this->m_currentSolution);

	return result;
}

long int GameModeCreative::time()
{
	return 0;
}

void GameModeCreative::afterNarrator()
{
	clearAll();

	m_widget = new QWidget();
	QGridLayout *gridLayout = new QGridLayout();
	m_widget->setLayout(gridLayout);

	int row = 0;
	int column = 0;
	int counter = 0;

	for (fairytale::ClipPackages::const_iterator packageIterator = app()->currentClipPackages().begin(); packageIterator != app()->currentClipPackages().end(); ++packageIterator)
	{
		ClipPackage *clipPackage = packageIterator.value();
		ClipPackage::Clips::iterator iterator = clipPackage->clips().begin();

		for (int i = 0; i < clipPackage->clips().size() && iterator != clipPackage->clips().end(); ++i, ++iterator)
		{
			Clip *clip = *iterator;
			const fairytale::ClipKey clipKey = fairytale::ClipKey(clipPackage->id(), clip->id());

			// never use a clip twice and only show expected clips
			if (this->app()->completeSolution().contains(clipKey) || (this->app()->requiresPerson() && !clip->isPerson()) || (!this->app()->requiresPerson() && clip->isPerson()))
			{
				continue;
			}

			ClipButton *button = new ClipButton(this->app(), clipKey);

			this->m_buttons.push_back(button);
			gridLayout->addWidget(button, row, column);
			button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			button->show(); // first show and resize
			button->updateGeometry();

			if ((counter + 1) % clipsPerRow == 0)
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
	}

	// now show buttons
	for (int i = 0; i < m_buttons.size(); ++i)
	{
		ClipButton *button = m_buttons[i];
		Clip *clip = app()->getClipByKey(button->clip());
		const QUrl url = this->app()->resolveClipUrl(clip->imageUrl());
#ifndef Q_OS_ANDROID
		const QString filePath = url.toLocalFile();
#else
		const QString filePath = url.url();
#endif

		button->iconButton()->setFile(filePath);
		connect(button->iconButton(), &IconButton::clicked, this, &GameModeCreative::clickCard);
		button->label()->setText(this->app()->description(this->app()->getClipByKey(this->app()->startPerson()), this->app()->turns(), clip, false));
		button->show();
	}

	m_scrollArea = new QScrollArea();
	m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_scrollArea->setWidget(m_widget);
	m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->app()->gameAreaLayout()->addWidget(m_scrollArea);
	this->app()->gameAreaLayout()->setAlignment(m_scrollArea, Qt::AlignCenter);

	m_finishButton = new QPushButton(this->app());
	m_finishButton->setText(tr("Complete Fairytale"));
	app()->gameAreaLayout()->addWidget(m_finishButton);
	app()->gameAreaLayout()->setAlignment(m_finishButton, Qt::AlignCenter);
	connect(m_finishButton, &QPushButton::clicked, this, &GameModeCreative::finish);
}

void GameModeCreative::nextTurn()
{
	clearAll();

	setState(State::Running);
}

void GameModeCreative::resume()
{
	foreach (ClipButton *button, this->m_buttons)
	{
		button->setEnabled(true);
	}

	m_finishButton->setEnabled(true);
}

void GameModeCreative::pause()
{
	foreach (ClipButton *button, this->m_buttons)
	{
		button->setEnabled(false);
	}

	m_finishButton->setEnabled(false);
}

void GameModeCreative::end()
{
	clearAll();
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

void GameModeCreative::clearAll()
{
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

	if (m_widget != nullptr)
	{
		delete m_widget;
		m_widget = nullptr;
	}

	if (m_scrollArea != nullptr)
	{
		delete m_scrollArea;
		m_scrollArea = nullptr;
	}
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

bool GameModeCreative::useMaxRounds()
{
	return false;
}

bool GameModeCreative::useDifficulty()
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

GameModeCreative::ClipButton::ClipButton(QWidget *parent, fairytale::ClipKey clip) : QWidget(parent), m_clip(clip), m_iconButton(new IconButton(this)), m_label(new QLabel(this))
{
	QVBoxLayout *layout = new QVBoxLayout();
	setLayout(layout);
	layout->addWidget(m_iconButton);
	layout->setAlignment(m_iconButton, Qt::AlignCenter);
	m_iconButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_iconButton->setMaximumSize(QSize(128, 128));
	m_iconButton->setMinimumSize(QSize(128, 128));
	layout->addWidget(m_label);
	layout->setAlignment(m_label, Qt::AlignCenter);
	// Keep the font size small, otherwise the buttons become too big, too.
	QFont font = m_label->font();
	font.setPointSize(15);
	m_label->setFont(font);
	m_label->setScaledContents(true);
	m_label->setWordWrap(true);
	m_label->setAlignment(Qt::AlignCenter);
}

