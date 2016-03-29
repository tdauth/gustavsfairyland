#include "fairytale.h"

#include <QtCore/QSettings>

#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtMultimedia/QMultimedia>

#include "clip.h"
#include "player.h"
#include "iconbutton.h"
#include "clipsdialog.h"
#include "clippackagedialog.h"
#include "clippackage.h"
#include "clippackageeditor.h"
#include "customfairytaledialog.h"
#include "gamemodeoneoutoffour.h"

void fairytale::newGame()
{
	this->m_timer.stop();

	clearSolution();

	this->m_paused = false;
	this->actionPauseGame->setText(tr("Pause Game"));
	this->m_player->pauseButton()->setText(tr("Pause Game"));

	// requires person in the first step always
	this->m_requiresPerson = true;
	this->m_turns = 0;

	ClipPackage *clipPackage = this->selectClipPackage();

	if (clipPackage != nullptr)
	{
		this->m_clipPackage = clipPackage;
		this->gameMode()->start();

		qDebug() << "start";

		nextTurn();

		setGameButtonsEnabled(true);
	}

	this->m_isRunning = true;
}

void fairytale::pauseGame()
{
	if (this->m_paused)
	{
		this->actionPauseGame->setText(tr("Pause Game"));
		this->m_player->pauseButton()->setText(tr("Pause Game"));
		this->m_paused = false;

		if (this->m_remainingTime > 0)
		{
			this->m_timer.start();

			this->gameMode()->resume();
		}
		else
		{
			this->m_player->mediaPlayer()->play();
		}
	}
	else
	{
		this->actionPauseGame->setText(tr("Continue Game"));
		this->m_player->pauseButton()->setText(tr("Continue Game"));
		this->m_paused = true;

		if (this->m_remainingTime > 0)
		{
			this->m_timer.stop();

			this->gameMode()->pause();
		}
		else
		{
			this->m_player->mediaPlayer()->pause();
		}
	}
}

void fairytale::showCustomFairytale()
{
	if (!this->m_paused)
	{
		pauseGame();
	}

	this->customFairytaleDialog()->show();
}

void fairytale::openClipsDialog()
{
	if (this->m_clipsDialog == nullptr)
	{
		this->m_clipsDialog = new ClipsDialog(this, this);
	}

	this->m_clipsDialog->fill(this->clipPackages());
	this->m_clipsDialog->show();
}

void fairytale::openEditor()
{
	if (this->m_editor == nullptr)
	{
		this->m_editor = new ClipPackageEditor(this, this);
	}

	this->m_editor->show();
}

ClipPackage* fairytale::selectClipPackage()
{
	if (this->m_clipPackageDialog == nullptr)
	{
		this->m_clipPackageDialog = new ClipPackageDialog(this);
	}

	this->m_clipPackageDialog->fill(this->clipPackages());

	if (this->m_clipPackageDialog->exec() == QDialog::Accepted)
	{
		return this->m_clipPackageDialog->clipPackage();
	}

	return nullptr;
}

fairytale::fairytale()
: m_turns(0)
, m_startPerson(nullptr)
, m_player(new Player(this, this))
, m_clipsDialog(nullptr)
, m_clipPackageDialog(nullptr)
, m_editor(nullptr)
, m_customFairytaleDialog(nullptr)
, m_remainingTime(0)
, m_requiresPerson(true)
, m_clipPackage(nullptr)
, m_completeSolutionIndex(0)
, m_playCompleteSolution(false)
, m_paused(false)
, m_isRunning(false)
, m_gameMode(new GameModeOneOutOfFour(this))
{
	this->m_player->hide();

	setupUi(this);


	connect(&this->m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));

	connect(actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));
	connect(actionPauseGame, SIGNAL(triggered()), this, SLOT(pauseGame()));
	connect(actionShowCustomFairytale, SIGNAL(triggered()), SLOT(showCustomFairytale()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionClips, SIGNAL(triggered()), this, SLOT(openClipsDialog()));
	connect(actionEditor, SIGNAL(triggered()), this, SLOT(openEditor()));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(this->m_player->mediaPlayer(), SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(finishNarrator(QMediaPlayer::State)));

	QSettings settings("fairytale");
	const int size = settings.beginReadArray("clipPackages");

	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		const QString filePath = settings.value("filePath").toString();

		ClipPackage *package = new ClipPackage(this);

		if (package->loadClipsFromFile(filePath))
		{
			this->m_clipPackages.push_back(package);
		}
		else
		{
			delete package;
		}
	}

	settings.endArray();
}

fairytale::~fairytale()
{
	QSettings settings("fairytale");
	settings.beginWriteArray("clipPackages", this->m_clipPackages.size());

	for (int i = 0; i < this->m_clipPackages.size(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("filePath", this->clipPackages()[i]->filePath());
	}

	settings.endArray();
}

void fairytale::playFinalClip(int index)
{
	this->m_completeSolutionIndex = index;

	this->m_player->playVideo(this, this->m_completeSolution[index]->narratorVideoUrl(), this->description(index, this->m_completeSolution[index]));
}

void fairytale::playFinalVideo()
{
	if (m_playCompleteSolution)
	{
		return;
	}

	this->m_playCompleteSolution = true;
	this->playFinalClip(0);
}

void fairytale::about()
{
	QMessageBox::information(this, tr("About"), tr("This game has been created by Tamino Dauth and Carsten Thomas. It is the best game you will ever play!"));
}

void fairytale::gameOver()
{
	this->gameMode()->end();
	this->clearSolution();
	this->setGameButtonsEnabled(false);
	this->timeLabel->setText("");
	this->descriptionLabel->setText("");
	QMessageBox::information(this, tr("Game over!"), tr("GAME OVER!"));
}

void fairytale::win()
{
	this->gameMode()->end();
	this->m_isRunning = false;
	QMessageBox::information(this, tr("WIN!"), tr("You won the game!!!!"), QMessageBox::Ok);

	if (this->customFairytaleDialog()->exec() == QDialog::Rejected)
	{
		this->clearSolution();
		this->setGameButtonsEnabled(false);
	}
}

void fairytale::nextTurn()
{
	this->timeLabel->setText(tr(""));
	this->descriptionLabel->setText("");

	if (m_turns > 1)
	{
		m_requiresPerson = !m_requiresPerson;
	}

	// calling this leads to the next turn in the game mode which should then call on onFinishTurn()
	this->gameMode()->nextTurn();

	switch (gameMode()->state())
	{
		case GameMode::State::Won:
		{
			this->win();

			break;
		}

		case GameMode::State::Running:
		{
			Clip *solution = this->gameMode()->solution();

			if (this->m_turns == 0)
			{
				this->m_startPerson = solution;
			}

			const QString description = this->description(this->m_turns, solution);

			/*
			 * Play the narrator clip for the current solution as hint.
			 */
			this->m_player->playVideo(this, solution->narratorVideoUrl(), description);

			break;
		}
	}
}

void fairytale::onFinishTurn()
{
	this->m_timer.stop();
	this->m_remainingTime = 0;
	this->m_turns++;
	addCurrentSolution();

	switch (this->gameMode()->state())
	{
		case GameMode::State::Won:
		{
			this->win();

			break;
		}

		case GameMode::State::Lost:
		{
			this->gameOver();

			break;
		}

		case GameMode::State::Running:
		{
			this->nextTurn();

			break;
		}
	}
}

void fairytale::clearSolution()
{
	this->m_completeSolutionIndex = 0;
	this->m_completeSolution.clear();

	foreach (QPushButton *button, this->m_completeSolutionButtons)
	{
		delete button;
	}

	this->m_completeSolutionButtons.clear();
	this->customFairytaleDialog()->clear();
}

void fairytale::setGameButtonsEnabled(bool enabled)
{
	actionPauseGame->setEnabled(enabled);
	actionShowCustomFairytale->setEnabled(enabled);
}

void fairytale::finishNarrator(QMediaPlayer::State state)
{
	if (state == QMediaPlayer::StoppedState)
	{
		if (!this->m_playCompleteSolution)
		{
			this->m_player->mediaPlayer()->stop();
			this->m_remainingTime = this->gameMode()->time();
			this->updateTimeLabel();
			// the description label helps to remember
			this->descriptionLabel->setText(this->description(this->m_turns, this->gameMode()->solution()));

			this->gameMode()->afterNarrator();

			// run every second
			this->m_timer.start(1000);
		}
		else if (this->m_playCompleteSolution && this->m_completeSolutionIndex + 1 < this->m_completeSolution.size())
		{
			qDebug() << "Play next final clip";
			this->playFinalClip(this->m_completeSolutionIndex + 1);
		}
		/*
		 * Stop playing the complete solution.
		 */
		else if (this->m_playCompleteSolution)
		{
			qDebug() << "Finished final clips";
			this->m_playCompleteSolution = false;
			this->m_completeSolutionIndex = 0;
		}
	}
}

void fairytale::timerTick()
{
	qDebug() << "Tick";
	this->m_remainingTime -= 1000;
	this->updateTimeLabel();

	if (this->m_remainingTime <= 0)
	{
		this->m_timer.stop();

		this->gameOver();
	}
}

void fairytale::updateTimeLabel()
{
	this->timeLabel->setText(tr("%1 Seconds").arg(QString::number(this->m_remainingTime / 1000)));
}

void fairytale::addCurrentSolution()
{
	this->customFairytaleDialog()->addClip(this->gameMode()->solution());
	this->m_completeSolution.push_back(this->gameMode()->solution());
}

QString fairytale::description(int turn, Clip *clip)
{
	QString description;

	if (clip->isPerson())
	{
		if (turn == 0)
		{
			description = tr("<b>%1</b>").arg(clip->description());
		}
		else if (turn == 1)
		{
			description = tr("and <b>%1</b>").arg(clip->description());
		}
		else
		{
			description = tr("%1 and <b>%2</b>").arg(this->m_startPerson->description()).arg(clip->description());
		}
	}
	else
	{
		description = tr("<b>%1</b>").arg(clip->description());
	}

	return description;
}

QUrl fairytale::resolveClipUrl(const QUrl& url) const
{
	if (!url.isRelative())
	{
		return url;
	}

	QUrl result = this->m_clipsDir;
	result.setUrl(this->m_clipsDir.url() + "/" + url.url());

	return result;
}

CustomFairytaleDialog* fairytale::customFairytaleDialog()
{
	if (this->m_customFairytaleDialog == nullptr)
	{
		this->m_customFairytaleDialog = new CustomFairytaleDialog(this, this);
	}

	return this->m_customFairytaleDialog;
}

#include "fairytale.moc"
