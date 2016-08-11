#include <QtCore/QSettings>

#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtMultimedia/QMultimedia>

#include "fairytale.h"
#include "clip.h"
#include "player.h"
#include "iconbutton.h"
#include "clipsdialog.h"
#include "clippackagedialog.h"
#include "gamemodedialog.h"
#include "clippackage.h"
#include "clippackageeditor.h"
#include "customfairytaledialog.h"
#include "gamemodeoneoutoffour.h"
#include "gamemodemoving.h"
#include "aboutdialog.h"
#include "settingsdialog.h"
#include "wondialog.h"

void fairytale::newGame()
{
	if (this->gameMode() != nullptr && this->isRunning())
	{
		this->cleanupGame();
	}

	this->m_timer.stop();

	clearSolution();

	this->m_paused = false;
	this->actionPauseGame->setText(tr("Pause Game"));
	this->m_player->pauseButton()->setText(tr("Pause Game (P)"));

	// requires person in the first step always
	this->m_requiresPerson = true;
	this->m_turns = 0;
	this->m_totalElapsedTime = 0;

	ClipPackage *clipPackage = this->selectClipPackage();

	if (clipPackage != nullptr)
	{
		this->m_clipPackage = clipPackage;

		this->m_gameMode = this->selectGameMode();

		if (m_gameMode != nullptr)
		{
			this->gameMode()->start();

			qDebug() << "start";

			nextTurn();

			setGameButtonsEnabled(true);
		}
	}

	this->m_isRunning = true;
}

void fairytale::pauseGame()
{
	if (this->m_paused)
	{
		this->actionPauseGame->setText(tr("Pause Game"));
		this->m_player->pauseButton()->setText(tr("Pause Game (P)"));
		this->m_paused = false;

		if (m_pausedTimer)
		{
			m_pausedTimer = false;
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
		this->m_player->pauseButton()->setText(tr("Continue Game (P)"));
		this->m_paused = true;

		m_pausedTimer = this->m_timer.isActive();

		if (m_pausedTimer)
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

void fairytale::cancelGame()
{
	// hold the game while asking to cancel
	this->pauseGame();

	if (QMessageBox::question(this, tr("Cancel Game?"), tr("Do you want to cancel the game?")) == QMessageBox::Yes)
	{
		this->cleanupGame();
	}
	else
	{
		// continue game
		this->pauseGame();
	}
}

void fairytale::showCustomFairytale()
{
	const bool pausedGame = !this->m_paused;

	if (pausedGame)
	{
		pauseGame();
	}

	this->customFairytaleDialog()->exec();

	// continue game
	if (pausedGame)
	{
		pauseGame();
	}
}

void fairytale::settings()
{
    if (m_settingsDialog == nullptr)
    {
		m_settingsDialog = new SettingsDialog(this, this);
    }

	m_settingsDialog->update();
    m_settingsDialog->exec();
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

GameMode* fairytale::selectGameMode()
{
	if (this->m_gameModeDialog == nullptr)
	{
		this->m_gameModeDialog = new GameModeDialog(this);
	}

	this->m_gameModeDialog->fill(this->gameModes());

	if (this->m_gameModeDialog->exec() == QDialog::Accepted)
	{
		return this->m_gameModeDialog->gameMode();
	}

	return nullptr;
}

fairytale::fairytale(Qt::WindowFlags flags)
: QMainWindow(0, flags)
, m_turns(0)
, m_startPerson(nullptr)
, m_player(new Player(this, this))
, m_settingsDialog(nullptr)
, m_clipsDialog(nullptr)
, m_clipPackageDialog(nullptr)
, m_gameModeDialog(nullptr)
, m_editor(nullptr)
, m_customFairytaleDialog(nullptr)
, m_remainingTime(0)
, m_requiresPerson(true)
, m_clipPackage(nullptr)
, m_completeSolutionIndex(0)
, m_playCompleteSolution(false)
, m_paused(false)
, m_pausedTimer(false)
, m_isRunning(false)
, m_audioPlayer(new QMediaPlayer(this))
, m_playNewSound(true)
, m_gameMode(nullptr)
, m_aboutDialog(nullptr)
, m_wonDialog(nullptr)
{
	this->m_player->hide();

	setupUi(this);

	connect(&this->m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));

	connect(actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));
	connect(actionPauseGame, SIGNAL(triggered()), this, SLOT(pauseGame()));
	connect(actionCancelGame, SIGNAL(triggered()), this, SLOT(cancelGame()));
	connect(actionShowCustomFairytale, SIGNAL(triggered()), SLOT(showCustomFairytale()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionSettings, &QAction::triggered, this, &fairytale::settings);
	connect(actionClips, SIGNAL(triggered()), this, SLOT(openClipsDialog()));
	connect(actionEditor, SIGNAL(triggered()), this, SLOT(openEditor()));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(this->m_player->mediaPlayer(), SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(finishNarrator(QMediaPlayer::State)));

	m_audioPlayer->setVolume(100);
	m_audioPlayer->setAudioRole(QAudio::GameRole);
	connect(this->m_audioPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(finishAudio(QMediaPlayer::State)));

	QSettings settings("fairytale");
	QDir defaultClipsDir;
#ifdef Q_OS_WIN
	const QDir currentDir(QDir::currentPath());
	defaultClipsDir = QDir(currentDir.filePath("../clips"));
#else
	defaultClipsDir = QDir("/usr/clips");
#endif
	// the default path is the "clips" sub directory
	m_clipsDir = QUrl::fromLocalFile(settings.value("clipsDir", defaultClipsDir.absolutePath()).toString());

	const int size = settings.beginReadArray("clipPackages");

	if (size > 0)
	{
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
	}
	// default package
	else
	{
		const QString filePath = defaultClipsDir.filePath("gustav.xml");
		const QFileInfo fileInfo(filePath);

		if (fileInfo.exists() && fileInfo.isReadable())
		{
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
		else
		{
			qDebug() << "Default clip package does not exist:" << fileInfo.absoluteFilePath();
		}
	}

	settings.endArray();

	m_gameModes.push_back(new GameModeMoving(this));
	m_gameModes.push_back(new GameModeOneOutOfFour(this));
}

fairytale::~fairytale()
{
	QSettings settings("fairytale");
	settings.setValue("clipsDir", m_clipsDir.toLocalFile());
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

	this->playSound(this->resolveClipUrl(this->m_completeSolution[index]->narratorVideoUrl()));
	this->m_player->playVideo(this, this->m_completeSolution[index]->videoUrl(), this->description(index, this->m_completeSolution[index]));
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
	this->aboutDialog()->exec();
}

void fairytale::gameOver()
{
	this->cleanupGame();
	QMessageBox::information(this, tr("Game over!"), tr("GAME OVER!"));
}

void fairytale::win()
{
	this->gameMode()->end();
	this->m_isRunning = false;

	this->wonDialog()->exec();

	// Show the custom fairytale dialog which allows the winner to watch his created fairytale.
	this->customFairytaleDialog()->exec(); // blocks until the dialog is closed

	// make sure everything is cleaned up
	this->cleanupAfterOneGame();
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

			this->m_remainingTime = this->gameMode()->time();
			this->updateTimeLabel();
			// the description label helps to remember
			this->descriptionLabel->setText(description);

			// play the sound for the inital character again
			if (solution->isPerson() && turns() > 1)
			{
				PlayerSoundData data;
				data.narratorSoundUrl = this->m_startPerson->narratorVideoUrl();
				data.description = this->description(0, this->m_startPerson);
				data.imageUrl = this->m_startPerson->imageUrl();
				data.prefix = true;
				this->queuePlayerSound(data);
			}

			// play the sound "and"
			if (solution->isPerson() && turns() > 0)
			{
				const QUrl narratorSoundUrl = QUrl("qrc:/resources/and.wav");
				PlayerSoundData data;
				data.narratorSoundUrl = narratorSoundUrl;
				data.description = tr("and");
				data.imageUrl = solution->imageUrl();
				data.prefix = true;
				this->queuePlayerSound(data);
			}

			/*
			 * Play the narrator sound for the current solution as hint.
			 */
			// Make sure that the current click sound ends before playing the narrator sound.
			PlayerSoundData data;
			data.narratorSoundUrl = solution->narratorVideoUrl();
			data.description = this->description(0, solution); // use always the stand alone description
			data.imageUrl = solution->imageUrl();
			data.prefix = false;
			this->queuePlayerSound(data);

			break;
		}
	}
}

void fairytale::onFinishTurn()
{
	// Store the time it took to finish the turn, ignore pauses!
	this->m_totalElapsedTime += this->gameMode()->time() - m_remainingTime;
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
	qDebug() << "Clearing solution";
	this->m_completeSolutionIndex = 0;
	qDebug() << "Clearing solution 1";
	this->m_completeSolution.clear();
	qDebug() << "Clearing solution 2";
	this->customFairytaleDialog()->clear();
	qDebug() << "Clearing solution 3";
}

void fairytale::setGameButtonsEnabled(bool enabled)
{
	actionPauseGame->setEnabled(enabled);
	actionCancelGame->setEnabled(enabled);
	actionShowCustomFairytale->setEnabled(enabled);
}

void fairytale::finishNarrator(QMediaPlayer::State state)
{
	if (state == QMediaPlayer::StoppedState)
	{
		// played narrator stuff
		if (!this->m_playCompleteSolution)
		{
			// Only react if the game mode has not been canceled
			if (this->gameMode()->state() == GameMode::State::Running)
			{
				// played a normal narrator clip, if the player has skipped one sound (a prefix sound for example) all sounds are skipped
				if (!this->m_player->isPrefix() || this->m_player->skipped() || m_playerSounds.empty())
				{
					this->m_player->mediaPlayer()->stop();
					this->m_player->hide(); // hide the player, otherwise one cannot play the game

					this->m_playerSounds.clear();

					this->gameMode()->afterNarrator();

					// run every second
					this->m_timer.start(1000);
				}
				// played only the word "and" or the first person sound then we always expect another sound
				else
				{
					const PlayerSoundData data = m_playerSounds.dequeue();
					this->m_player->playSound(this, data.narratorSoundUrl, data.description, data.imageUrl, data.prefix);
				}
			}
		}
		// Play the next final clip of the complete solution.
		else if (this->m_playCompleteSolution && this->m_completeSolutionIndex + 1 < this->m_completeSolution.size())
		{
			qDebug() << "Play next final clip";
			// next time play the following clip
			this->m_completeSolutionIndex++;
			this->playFinalClip(this->m_completeSolutionIndex);
		}
		/*
		 * Stop playing the complete solution.
		 */
		else if (this->m_playCompleteSolution)
		{
			qDebug() << "Finished final clips";
			this->m_playCompleteSolution = false;
			this->m_completeSolutionIndex = 0;

			// The dialog has to disappear, after the player watched all final clips.
			this->m_player->hide();
			this->customFairytaleDialog()->hide();
			this->cleanupAfterOneGame();
		}
	}
}

void fairytale::finishAudio(QMediaPlayer::State state)
{
	if (state == QMediaPlayer::StoppedState)
	{
		m_playNewSound = true;

		// If any player sound is queued play it next.
		if (!m_playerSounds.empty())
		{
			queuePlayerSound(m_playerSounds.dequeue());
		}
	}
}

void fairytale::timerTick()
{
	const QTimer *sender = dynamic_cast<QTimer*>(QObject::sender());
	//qDebug() << "Tick";
	this->m_remainingTime -= sender->interval();
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
	// Make sure the label is update immediately.
	this->timeLabel->repaint();
}

void fairytale::addCurrentSolution()
{
	this->customFairytaleDialog()->addClip(this->gameMode()->solution());
	this->m_completeSolution.push_back(this->gameMode()->solution());
}

QString fairytale::description(int turn, Clip *clip, bool markBold)
{
	QString description;

	if (clip->isPerson())
	{
		if (turn == 0)
		{
			if (markBold)
			{
				description = tr("<b>%1</b>").arg(clip->description());
			}
			else
			{
				description = tr("%1").arg(clip->description());
			}
		}
		else if (turn == 1)
		{
			if (markBold)
			{
				description = tr("und <b>%1</b>").arg(clip->description());
			}
			else
			{
				description = tr("und %1").arg(clip->description());
			}
		}
		else
		{
			if (markBold)
			{
				description = tr("%1 und <b>%2</b>").arg(this->m_startPerson->description()).arg(clip->description());
			}
			else
			{
				description = tr("%1 und %2").arg(this->m_startPerson->description()).arg(clip->description());
			}
		}
	}
	else
	{
		if (markBold)
		{
			description = tr("<b>%1</b>").arg(clip->description());
		}
		else
		{
			description = tr("%1").arg(clip->description());
		}
	}

	return description;
}

bool fairytale::playSound(const QUrl &url)
{
	if (m_playNewSound)
	{
		m_playNewSound = false;
		m_audioPlayer->setMedia(url);
		m_audioPlayer->setVolume(50);
		m_audioPlayer->play();

		return true;
	}

	return false;
}

void fairytale::queuePlayerSound(const PlayerSoundData &data)
{
	// TODO get all states when a media is still being played or loaded etc.
	if (!m_playNewSound || this->m_player->mediaPlayer()->state() != QMediaPlayer::StoppedState)
	{
		this->m_playerSounds.push_back(data);
	}
	else
	{
		this->m_player->playSound(this, data.narratorSoundUrl, data.description, data.imageUrl, data.prefix);
	}
}

void fairytale::cleanupGame()
{
	this->m_playerSounds.clear();
	this->gameMode()->end(); // end the game mode before stopping the player, the player has to know that the game mode is ended
	this->m_player->mediaPlayer()->stop();
	this->m_player->hide();
	this->cleanupAfterOneGame();
	this->timeLabel->setText("");
	this->descriptionLabel->setText("");
}

void fairytale::cleanupAfterOneGame()
{
	this->clearSolution();
	qDebug() << "Before disabling game buttons";
	this->setGameButtonsEnabled(false);
	qDebug() << "After disabling game buttons";
}

QUrl fairytale::resolveClipUrl(const QUrl& url) const
{
	if (!url.isRelative())
	{
		qDebug() << "Absolute: " << url;
		return url;
	}

	QUrl result = this->m_clipsDir;
	result.setUrl(this->m_clipsDir.url() + "/" + url.url());
	qDebug() << "Resolved: " << result;

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

AboutDialog* fairytale::aboutDialog()
{
	if (this->m_aboutDialog == nullptr)
	{
		this->m_aboutDialog = new AboutDialog(this, this);
	}

	return this->m_aboutDialog;
}

WonDialog* fairytale::wonDialog()
{
	if (this->m_wonDialog == nullptr)
	{
		this->m_wonDialog = new WonDialog(this, this);
	}

	return this->m_wonDialog;
}


#include "fairytale.moc"
