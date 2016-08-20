#include <iostream>

#include <QtCore/QSettings>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QTouchDevice>

#include <QtMultimedia/QMultimedia>

#include "fairytale.h"
#include "clip.h"
#include "bonusclip.h"
#include "player.h"
#include "iconbutton.h"
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
#include "gameoverdialog.h"
#include "highscores.h"
#include "customfairytale.h"

void fairytale::newGame()
{
	if (this->gameMode() != nullptr && this->isGameRunning())
	{
		this->cleanupGame();
	}

	if (this->isTimerRunning())
	{
		this->m_timer.stop();
	}

	clearSolution();

	ClipPackage *clipPackage = this->selectClipPackage();

	if (clipPackage != nullptr)
	{
		GameMode *gameMode = this->selectGameMode();

		if (m_gameMode != nullptr)
		{
			startNewGame(clipPackage, gameMode);
		}
	}
}

void fairytale::pauseGameAction()
{
	if (this->isGamePaused())
	{
		this->resumeGame();
	}
	else
	{
		this->pauseGame();
	}
}

void fairytale::cancelGame()
{
	if (!this->isGameRunning())
	{
		qDebug() << "Invalid call of cancelGame()";

		return;
	}

	// hold the game while asking to cancel
	const bool pause = !this->isGamePaused();

	if (pause)
	{
		this->pauseGame();
	}

	if (QMessageBox::question(this, tr("Cancel Game?"), tr("Do you want to cancel the game?")) == QMessageBox::Yes)
	{
		this->cleanupGame();
	}
	else if (pause)
	{
		// continue game
		this->resumeGame();
	}
}

void fairytale::showCustomFairytale()
{
	const bool pausedGame = this->isGameRunning() && !this->isGamePaused();

	if (pausedGame)
	{
		pauseGame();
	}

	this->customFairytaleDialog()->exec();

	// continue game
	if (pausedGame)
	{
		resumeGame();
	}
}

void fairytale::settings()
{
	const bool pausedGame = this->isGameRunning() && !this->isGamePaused();

	if (pausedGame)
	{
		pauseGame();
	}

	settingsDialog()->update();
	settingsDialog()->exec();

	// continue game
	if (pausedGame)
	{
		resumeGame();
	}
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

	this->m_gameModeDialog->fill(this->gameModes(), this);

	if (this->m_gameModeDialog->exec() == QDialog::Accepted)
	{
		return this->m_gameModeDialog->gameMode();
	}

	return nullptr;
}

bool fairytale::hasTouchDevice()
{
	foreach (const QTouchDevice *device, QTouchDevice::devices())
	{
		if (device->capabilities() & QTouchDevice::MouseEmulation)
		{
			return true;
		}
	}

	return false;
}

QString fairytale::localeToName(const QString &locale)
{
	if (locale == "de")
	{
		return "Deutsch";
	}
	else if (locale == "en")
	{
		return "English";
	}

	return locale;
}

void fairytale::startNewGame(ClipPackage *clipPackage, GameMode *gameMode)
{
	if (this->gameMode() != nullptr && this->isGameRunning())
	{
		this->cleanupGame();
	}

	if (this->isTimerRunning())
	{
		this->m_timer.stop();
	}

	clearSolution();

	this->m_paused = false;
	this->actionPauseGame->setText(tr("Pause Game"));
	this->m_player->pauseButton()->setText(tr("Pause Game (P)"));

	// requires person in the first step always
	this->m_requiresPerson = true;
	this->m_turns = 0;
	this->m_totalElapsedTime = 0;
	this->menuButtonsWidget->hide();
	this->gameAreaWidget->show();
	this->descriptionLabel->show();
	this->timeLabel->show();

	this->m_clipPackage = clipPackage;
	this->m_gameMode = gameMode;
	this->gameMode()->start();
	qDebug() << "start";
	this->m_isRunning = true;
	nextTurn();

	setGameButtonsEnabled(true);
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
, m_isRunningTimer(false)
, m_pausedMediaPlayer(false)
, m_isPlayingMediaPlayer(false)
, m_isRunning(false)
, m_audioPlayer(new QMediaPlayer(this))
, m_playNewSound(true)
, m_musicPlayer(new QMediaPlayer(this))
, m_gameMode(nullptr)
, m_aboutDialog(nullptr)
, m_wonDialog(nullptr)
, m_gameOverDialog(nullptr)
, m_highScores(new HighScores(this, this))
, m_playingBonusClip(false)
, m_playingCustomFairytale(nullptr)
, m_customFairytaleIndex(0)
{
	this->m_player->hide();

	setupUi(this);

	connect(&this->m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));

	connect(actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));
	connect(actionPauseGame, SIGNAL(triggered()), this, SLOT(pauseGameAction()));
	connect(actionCancelGame, SIGNAL(triggered()), this, SLOT(cancelGame()));
	connect(actionShowCustomFairytale, SIGNAL(triggered()), SLOT(showCustomFairytale()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionSettings, &QAction::triggered, this, &fairytale::settings);
	connect(actionEditor, SIGNAL(triggered()), this, SLOT(openEditor()));
	connect(actionHighScores, &QAction::triggered, this, &fairytale::showHighScores);
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(quickGamePushButton, &QPushButton::clicked, this, &fairytale::quickGame);
	connect(highScoresPushButton, &QPushButton::clicked, this, &fairytale::showHighScores);
	connect(quitPushButton, &QPushButton::clicked, this, &fairytale::close);

#ifndef Q_OS_ANDROID
	connect(this->m_player->mediaPlayer(), SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(finishNarrator(QMediaPlayer::State)));
#else
	// Android uses QML and the QML type MediaPlayer emits a signal without a parameter. The state must be checked in the slot itself.
	connect(this->m_player->mediaPlayer(), SIGNAL(playbackStateChanged()), this, SLOT(finishNarratorAndroid()));
#endif
	connect(this->m_player, &Player::finishVideoAndSounds, this, &fairytale::onFinishVideoAndSounds);

	const QDir dir(translationsDir());

	// Try to load the current locale. If no translation file exists it will remain English.
	QString locale = QLocale::system().name();
	locale.truncate(locale.lastIndexOf('_'));

	QAction *currentLocaleAction = nullptr;

	foreach (const QFileInfo &languageFile, dir.entryInfoList(QStringList("*.qm")))
	{
		QAction *action = new QAction(localeToName(languageFile.baseName()), this);
		action->setCheckable(true);
		connect(action, SIGNAL(triggered()), this, SLOT(changeLanguage()));
		menuLanguage->addAction(action);
		m_translationFileNames.insert(action, languageFile.baseName());

		if (languageFile.baseName() == locale)
		{
			currentLocaleAction = action;
		}
	}

	m_audioPlayer->setAudioRole(QAudio::GameRole);
	m_audioPlayer->setVolume(100);
	connect(this->m_audioPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(finishAudio(QMediaPlayer::State)));

	m_audioPlayer->setAudioRole(QAudio::GameRole);
	connect(this->m_musicPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(finishMusic(QMediaPlayer::State)));

	GameModeMoving *gameModeMoving = new GameModeMoving(this);
	m_gameModes.insert(gameModeMoving->id(), gameModeMoving);
	GameModeOneOutOfFour *gameModeOneOutOfFour = new GameModeOneOutOfFour(this);
	m_gameModes.insert(gameModeOneOutOfFour->id(), new GameModeOneOutOfFour(this));

	QSettings settings("TaCaProduction", "gustavsfairyland");

	// Loads the clips directory and applies it. Besides loads the clip packages and sets the default clip package if none is found.
	settingsDialog()->load(settings);

	const int highScoresSize = settings.beginReadArray("highscores");
	qDebug() << "Read high scores:" << highScoresSize;

	for (int i = 0; i < highScoresSize; ++i)
	{
		settings.setArrayIndex(i);
		HighScore highScore(settings.value("name").toString(), settings.value("package").toString(), settings.value("gameMode").toString(), settings.value("rounds").toInt(), settings.value("time").toInt());
		this->highScores()->addHighScore(highScore);
	}

	settings.endArray();

	qApp->installTranslator(&m_translator);

	loadLanguage(locale);

	if (currentLocaleAction != nullptr)
	{
		currentLocaleAction->setChecked(true);
	}

	// Initial cleanup.
	cleanupGame();

	// start the background music from Mahler
	startMusic();
}

fairytale::~fairytale()
{
	QSettings settings("TaCaProduction", "gustavsfairyland");

	settingsDialog()->save(settings);

	settings.beginWriteArray("highscores");
	int i = 0;

	foreach (const HighScores::HighScoreList &highScoreList, this->highScores()->highScores().values())
	{
		foreach (const HighScore &highScore, highScoreList)
		{
			settings.setArrayIndex(i);
			settings.setValue("name", highScore.name());
			settings.setValue("package", highScore.package());
			settings.setValue("gameMode", highScore.gameMode());
			settings.setValue("rounds", highScore.rounds());
			settings.setValue("time", highScore.time());

			++i;
		}
	}

	qDebug() << "Wrote high scores:" << i;

	settings.endArray();

	qApp->removeTranslator(&m_translator);
}

QString fairytale::defaultClipsDirectory() const
{
#ifdef Q_OS_WIN
	/*
	 * The player usually starts the binary via a Desktop link which runs it in the directory:
	 * "C:\Program Files (x86)\gustavsfairyland"
	 * But the player can also directly start the .exe file in the bin folder.
	 * Therefore one must get the file path to the binary file and not use QDir::currentPath().
	 */
	return QCoreApplication::applicationDirPath() + "/../share/gustavsfairyland/clips";
#elif defined(Q_OS_ANDROID)
	return QString("assets:/clips");
#else
	return QString("/usr/share/gustavsfairyland/clips");
#endif
}

void fairytale::playFinalClip(int index)
{
	this->m_completeSolutionIndex = index;
	Clip *solution = this->m_completeSolution[index];

	// play all sounds parallel to the clip

	// play the sound for the inital character again
	if (solution->isPerson() && index > 1)
	{
		this->m_player->playParallelSound(this, this->resolveClipUrl(this->m_startPerson->narratorUrl()));
	}

	// play the sound "and"
	if (solution->isPerson() && index > 0)
	{
		const QUrl narratorSoundUrl = QUrl("qrc:/resources/and.wav");

		this->m_player->playParallelSound(this, narratorSoundUrl);
	}

	this->m_player->playParallelSound(this, this->resolveClipUrl(solution->narratorUrl()));
	this->m_player->playVideo(this, solution->videoUrl(), this->description(index, solution));
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

void fairytale::playCustomFairytaleClip(int index)
{
	this->m_customFairytaleIndex = index;
	const QString packageId = this->m_playingCustomFairytale->packageId();

	ClipPackages::iterator iterator = this->m_clipPackages.find(packageId);

	if (iterator != this->m_clipPackages.end())
	{
		ClipPackage *clipPackage = iterator.value();
		const QString clipId = this->m_playingCustomFairytale->clipIds()[index];
		ClipPackage::Clips::iterator clipIterator = clipPackage->clips().find(clipId);

		if (clipIterator != clipPackage->clips().end())
		{
			Clip *solution = clipIterator.value();

			// play all sounds parallel to the clip

			// play the sound for the inital character again
			if (solution->isPerson() && index > 1)
			{
				this->m_player->playParallelSound(this, this->resolveClipUrl(this->m_startPerson->narratorUrl()));
			}

			// play the sound "and"
			if (solution->isPerson() && index > 0)
			{
				const QUrl narratorSoundUrl = QUrl("qrc:/resources/and.wav");

				this->m_player->playParallelSound(this, narratorSoundUrl);
			}

			this->m_player->playParallelSound(this, this->resolveClipUrl(solution->narratorUrl()));
			this->m_player->playVideo(this, solution->videoUrl(), this->description(index, solution));
		}
	}
}

void fairytale::playCustomFairytale(CustomFairytale *customFairytale)
{
	if (m_playingCustomFairytale != nullptr || this->isGameRunning() || this->m_playCompleteSolution)
	{
		qDebug() << "Invalid playCustomFairytale() call";

		return;
	}

	this->m_playingCustomFairytale = customFairytale;
	this->playCustomFairytaleClip(0);
}

void fairytale::showHighScores()
{
	this->highScores()->exec();
}

void fairytale::about()
{
	this->aboutDialog()->exec();
}

void fairytale::quickGame()
{
	if (this->isGameRunning() || this->clipPackages().isEmpty() || this->gameModes().isEmpty())
	{
		qDebug() << "Cant start a quick game!";
		qDebug() << "Is running:" << this->isGameRunning();

		return;
	}

	// Start with the first available stuff.
	ClipPackage *clipPackage = this->defaultClipPackage();
	GameMode *gameMode = this->defaultGameMode();

	startNewGame(clipPackage, gameMode);
}

void fairytale::retry()
{
	if (this->clipPackages().isEmpty() || this->gameModes().isEmpty())
	{
		return;
	}

	// Start with the first available stuff.
	ClipPackage *clipPackage = this->clipPackage();
	GameMode *gameMode = this->gameMode();

	this->startNewGame(clipPackage, gameMode);
}

QDir fairytale::translationsDir() const
{
#ifdef DEBUG
	const QDir translationsDir = QDir(QDir::currentPath());
#elif defined(Q_OS_WIN)
	/*
	 * The player usually starts the binary via a Desktop link which runs it in the directory:
	 * "C:\Program Files (x86)\gustavsfairyland"
	 * But the player can also directly start the .exe file in the bin folder.
	 * Therefore one must get the file path to the binary file and not use QDir::currentPath().
	 */
	const QDir translationsDir = QCoreApplication::applicationDirPath() + "/../share/gustavsfairyland/translations";
	qDebug() << "Windows translation dir:" << translationsDir;
#elif defined(Q_OS_ANDROID)
	const QDir translationsDir = QDir("assets:/translations");
#else
	const QDir translationsDir = QDir("/usr/share/gustavsfairyland/translations");
#endif

	return translationsDir;
}

void fairytale::loadLanguage(const QString &language)
{
	qDebug() << "Translation directory: " << translationsDir().path();
	qDebug() << "Translation: " << language;

	if (m_translator.load(language, translationsDir().path()))
	{
		qDebug() << "Loaded file!";
		qDebug() << "File loaded:" << language;
	}
	else
	{
		qDebug() << "Did not load file: " << language;
		qWarning() << "File not loaded";
	}

	// Update to this language even if the translation file is invalid.
	m_currentTranslation = language;
}

void fairytale::gameOver()
{
	this->gameMode()->end();
	this->m_isRunning = false;

	this->gameOverDialog()->exec();

	// Show the custom fairytale dialog which allows the loser to watch his created fairytale.
	this->customFairytaleDialog()->exec();

	// dont clean up on retry
	// make sure everything is cleaned up
	if (!this->customFairytaleDialog()->clickedRetry())
	{
		this->cleanupAfterOneGame();
	}
	// prevents recursive calls in dialog
	else
	{
		this->retry();
	}
}

void fairytale::win()
{
	this->gameMode()->end();
	this->m_isRunning = false;

	this->wonDialog()->exec();

	QString name = qgetenv("USER");

	if (name.isEmpty())
	{
		name = qgetenv("USERNAME");
	}

	HighScore highScore(name, this->clipPackage()->id(), this->gameMode()->id(), this->m_turns, this->m_totalElapsedTime);
	this->highScores()->addHighScore(highScore);

	// Show the custom fairytale dialog which allows the winner to watch his created fairytale.
	this->customFairytaleDialog()->exec();

	// dont clean up on retry
	// make sure everything is cleaned up
	if (!this->customFairytaleDialog()->clickedRetry())
	{
		this->cleanupAfterOneGame();
	}
	// prevents recursive calls in dialog
	else
	{
		this->retry();
	}
}

bool fairytale::isGamePaused() const
{
	return this->m_paused;
}

void fairytale::pauseGame()
{
	if (this->isGamePaused() || !this->isGameRunning())
	{
		qDebug() << "Warning: Invalid call of pauseGame()";

		return;
	}

	this->actionPauseGame->setText(tr("Continue Game"));
	this->m_player->pauseButton()->setText(tr("Continue Game (P)"));
	this->m_paused = true;

	if (this->isMediaPlayerPlaying())
	{
		this->m_player->pause();
	}
	else
	{
		this->pauseTimer();
		this->gameMode()->pause();
	}
}

void fairytale::resumeGame()
{
	if (!this->isGamePaused() || !this->isGameRunning())
	{
		qDebug() << "Warning: Invalid call of resumeGame()";

		return;
	}

	this->actionPauseGame->setText(tr("Pause Game"));
	this->m_player->pauseButton()->setText(tr("Pause Game (P)"));
	this->m_paused = false;

	if (this->isMediaPlayerPaused())
	{
		this->m_player->play();
	}
	else
	{
		this->resumeTimer();
		this->gameMode()->resume();
	}
}

bool fairytale::isTimerPaused() const
{
	return this->m_pausedTimer;
}

bool fairytale::isTimerRunning() const
{
	return this->m_isRunningTimer;
}

void fairytale::pauseTimer()
{
	if (this->isTimerPaused())
	{
		qDebug() << "Warning: Invalid call of pauseTimer()";

		return;
	}

	this->m_timer.stop();
	this->m_isRunningTimer = false;
	this->m_pausedTimer = true;
}

void fairytale::resumeTimer()
{
	if (!this->isTimerPaused())
	{
		qDebug() << "Warning: Invalid call of resumeTimer()";

		return;
	}

	this->m_pausedTimer = false;
	this->m_isRunningTimer = true;
	this->m_timer.start();
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

		// Should never happen
		case GameMode::State::Lost:
		{
			this->gameOver();

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
			this->descriptionLabel->clear();
			this->timeLabel->clear();

			qDebug() << "Queue the sounds.";

			// play the sound for the inital character again
			if (solution->isPerson() && turns() > 1)
			{
				PlayerSoundData data;
				data.narratorSoundUrl = this->m_startPerson->narratorUrl();
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
			data.narratorSoundUrl = solution->narratorUrl();
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

	foreach (QAction *action, this->m_bonusClipActions.keys())
	{
		action->setEnabled(!enabled);
	}
}

#ifdef Q_OS_ANDROID
void fairytale::finishNarratorAndroid()
{
	this->finishNarrator(this->m_player->state());

}
#endif

void fairytale::finishNarrator(QMediaPlayer::State state)
{
	qDebug() << "Finish narrator with state:" << state;

	switch (state)
	{
		case QMediaPlayer::StoppedState:
		{
			// is handled in onFinishVideoAndSounds()
			break;
		}

		case QMediaPlayer::PausedState:
		{
			this->m_pausedMediaPlayer = true;

			break;
		}

		case QMediaPlayer::PlayingState:
		{
			this->m_isPlayingMediaPlayer = true;
			this->m_pausedMediaPlayer = false;

			break;
		}
	}
}

void fairytale::onFinishVideoAndSounds()
{
	qDebug() << "Finish video and sounds";

	if (!this->m_playingBonusClip)
	{
		this->m_isPlayingMediaPlayer = false;
		this->m_pausedMediaPlayer = false;

		// played narrator stuff
		if (!this->m_playCompleteSolution && m_playingCustomFairytale == nullptr)
		{
			// Only react if the game mode has not been canceled
			if (this->gameMode()->state() == GameMode::State::Running)
			{
				// played a normal narrator clip, if the player has skipped one sound (a prefix sound for example) all sounds are skipped
				if (!this->m_player->isPrefix() || this->m_player->skipped() || m_playerSounds.empty())
				{
					this->m_player->stop();
					this->m_player->hide(); // hide the player, otherwise one cannot play the game

					this->m_playerSounds.clear();

					this->updateTimeLabel();
					// the description label helps to remember
					this->descriptionLabel->setText(this->description(this->m_turns, this->gameMode()->solution()));

					this->gameMode()->afterNarrator();

					// run every second
					this->m_isRunningTimer = true;
					this->m_pausedTimer = false;
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
		else if (this->m_playCompleteSolution)
		{
			if (this->m_completeSolutionIndex + 1 < this->m_completeSolution.size())
			{
				qDebug() << "Play next final clip";
				// next time play the following clip
				this->m_completeSolutionIndex++;
				this->playFinalClip(this->m_completeSolutionIndex);
			}
			/*
			* Stop playing the complete solution.
			*/
			else
			{
				qDebug() << "Finished final clips";
				this->m_playCompleteSolution = false;
				this->m_completeSolutionIndex = 0;

				// The dialog has to disappear, after the player watched all final clips.
				this->m_player->hide();
				// Dont hide the custom fairytale. The player should have the change to save or rewatch it.
			}
		}
		// Play the next clip of a custom fairytale
		else if (m_playingCustomFairytale != nullptr)
		{
			if (this->m_customFairytaleIndex + 1 < this->m_playingCustomFairytale->clipIds().size())
			{
				qDebug() << "Play next custom fairytale clip";
				// next time play the following clip
				this->m_customFairytaleIndex++;
				this->playCustomFairytaleClip(this->m_customFairytaleIndex);
			}
			/*
			* Stop playing the custom fairytale.
			*/
			else
			{
				qDebug() << "Finished custom fairytale clips";
				this->m_playingCustomFairytale = nullptr;
				this->m_customFairytaleIndex = 0;

				// The dialog has to disappear, after the player watched all final clips.
				this->m_player->hide();
			}
		}
	}
	else
	{
		this->m_playingBonusClip = false;
		this->m_player->hide();
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

void fairytale::startMusic()
{
	// TODO add to package XML file, each package can have its own background music
	QList<QUrl> urls;
	urls.push_back(QUrl("./music/01.PSO020103-Mahler-5-I.mp3"));
	urls.push_back(QUrl("./music/02.PSO020103-Mahler-5-II.mp3"));
	urls.push_back(QUrl("./music/03.PSO020103-Mahler-5-III.mp3"));
	urls.push_back(QUrl("./music/04.PSO020103-Mahler-5-IV.mp3"));
	urls.push_back(QUrl("./music/05.PSO020103-Mahler-5-V.mp3"));
	const QUrl url = urls.at(qrand() % urls.size());
	const QUrl musicUrl = this->resolveClipUrl(url);
	std::cerr << "Play music:" << musicUrl.toString().toStdString() << std::endl;
	m_musicPlayer->setMedia(musicUrl);
	m_musicPlayer->play();
	// Music should not be annoyingly loud.
	m_musicPlayer->setVolume(50);
}

void fairytale::finishMusic(QMediaPlayer::State state)
{
	if (state == QMediaPlayer::StoppedState)
	{
		// restart another music
		startMusic();
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
				description = tr("and <b>%1</b>").arg(clip->description());
			}
			else
			{
				description = tr("and %1").arg(clip->description());
			}
		}
		else
		{
			if (markBold)
			{
				description = tr("%1 and <b>%2</b>").arg(this->m_startPerson->description()).arg(clip->description());
			}
			else
			{
				description = tr("%1 and %2").arg(this->m_startPerson->description()).arg(clip->description());
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
	if (!m_playNewSound || this->m_player->state() != QMediaPlayer::StoppedState)
	{
		this->m_playerSounds.push_back(data);
	}
	else
	{
		this->m_player->playSound(this, data.narratorSoundUrl, data.description, data.imageUrl, data.prefix);
	}
}

void fairytale::changeEvent(QEvent* event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			std::cerr << "Retranslate UI" << std::endl;
			this->retranslateUi(this);
			//this->aboutDialog()->retranslateUi(this->aboutDialog());

			break;
		}

		// this event is send, if the system, language changes
		case QEvent::LocaleChange:
		{
			QString locale = QLocale::system().name();
			locale.truncate(locale.lastIndexOf('_'));
			loadLanguage(locale);

			break;
		}

		default:
		{
			break;
		}
	}

	QMainWindow::changeEvent(event);
}

void fairytale::cleanupGame()
{
	this->m_playerSounds.clear();

	// this method is also called in the beginning when there is no gamemode
	if (this->gameMode() != nullptr)
	{
		this->gameMode()->end(); // end the game mode before stopping the player, the player has to know that the game mode is ended
	}

	this->m_player->stop();
	this->m_player->hide();
	this->cleanupAfterOneGame();
	this->m_isRunning = false;
}

void fairytale::cleanupAfterOneGame()
{
	this->clearSolution();
	qDebug() << "Before disabling game buttons";
	this->setGameButtonsEnabled(false);
	qDebug() << "After disabling game buttons";

	this->timeLabel->setText("");
	this->timeLabel->hide();
	this->descriptionLabel->setText("");
	this->descriptionLabel->hide();
	this->gameAreaWidget->hide();
	this->menuButtonsWidget->show();
	// Make sure all paint stuff from the game mode disappears.
	this->repaint();
}

QUrl fairytale::resolveClipUrl(const QUrl &url) const
{
	if (!url.isRelative())
	{
		qDebug() << "Absolute: " << url;

		return url;
	}

#ifndef Q_OS_ANDROID
	QUrl result = this->m_clipsDir;
	result.setUrl(this->m_clipsDir.url() + "/" + url.url());
#else
	// Android uses assets as local files and Qt provides this protocol for these files
	QString relativePath = url.toString();

	if (relativePath.startsWith("./"))
	{
		relativePath = relativePath.mid(2);
	}

	qDebug() << "Clips dir:" << this->m_clipsDir.toString();
	QUrl result(this->m_clipsDir.toString() + "/" + relativePath);
#endif

	qDebug() << "Resolved: " << result;

	return result;
}

SettingsDialog* fairytale::settingsDialog()
{
	if (m_settingsDialog == nullptr)
	{
		m_settingsDialog = new SettingsDialog(this, this);
	}

	return m_settingsDialog;
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

GameOverDialog* fairytale::gameOverDialog()
{
	if (this->m_gameOverDialog == nullptr)
	{
		this->m_gameOverDialog = new GameOverDialog(this, this);
	}

	return this->m_gameOverDialog;
}

void fairytale::addClipPackage(ClipPackage* package)
{
	this->m_clipPackages.insert(package->id(), package);

	foreach (BonusClip *bonusClip, package->bonusClips())
	{
		QAction *action = menuAchievements->addAction(bonusClip->description());
		connect(action, &QAction::triggered, this, &fairytale::playBonusClip);
		this->m_bonusClipActions.insert(action, bonusClip);
	}
}

void fairytale::playBonusClip()
{
	if (m_playingBonusClip || this->isGameRunning())
	{
		return;
	}

	QAction *action = dynamic_cast<QAction*>(sender());

	BonusClipActions::iterator iterator = this->m_bonusClipActions.find(action);

	if (iterator != this->m_bonusClipActions.end())
	{
		this->m_playingBonusClip = true;
		this->m_player->playBonusVideo(this, iterator.value()->videoUrl(), iterator.value()->description());
	}
}

void fairytale::changeLanguage()
{
	QAction *action = dynamic_cast<QAction*>(sender());

	std::cerr << "Change language to: " << action->text().toStdString() << std::endl;

	TranslationFileNames::const_iterator iterator = m_translationFileNames.find(action);

	if (iterator != m_translationFileNames.end())
	{
		loadLanguage(iterator.value());

		// uncheck all other languages
		TranslationFileNames::const_iterator i = m_translationFileNames.begin();

		while (i != m_translationFileNames.end())
		{
			if (i.key() != action)
			{
				i.key()->setChecked(false);
			}

			++i;
		}
	}
	else
	{
		action->setChecked(false);
	}
}

void fairytale::removeClipPackage(ClipPackage* package)
{
	this->m_clipPackages.remove(package->id());
}

GameMode* fairytale::defaultGameMode() const
{
	// prefer floating clips in a room
	GameModes::const_iterator iterator = this->gameModes().find("pagesontheground");

	if (iterator != this->gameModes().end())
	{
		return iterator.value();
	}

	return this->gameModes().first();
}

ClipPackage* fairytale::defaultClipPackage() const
{
	// prefer the package of Gustav's Fairyland
	ClipPackages::const_iterator iterator = this->clipPackages().find("gustav");

	if (iterator != this->clipPackages().end())
	{
		return iterator.value();
	}

	return this->clipPackages().first();
}

bool fairytale::loadDefaultClipPackage()
{
#ifndef Q_OS_ANDROID
	const QDir defaultClipsDir(this->defaultClipsDirectory());
	const QString filePath = defaultClipsDir.filePath("gustav.xml");
	const QFileInfo fileInfo(filePath);

	std::cerr << "Loading default clip package from: " << filePath.toStdString() << std::endl;

	if (fileInfo.exists() && fileInfo.isReadable())
	{
		ClipPackage *package = new ClipPackage(this);

		if (package->loadClipsFromFile(filePath))
		{
			this->addClipPackage(package);

			return true;
		}
		else
		{
			delete package;
			package = nullptr;
		}
	}
	else
	{
		qDebug() << "Default clip package does not exist:" << fileInfo.absoluteFilePath();
	}
#else
	ClipPackage *package = new ClipPackage(this);

	const QString fileName("assets:/clips/gustav.xml");

	qDebug() << "Opening package:" << fileName;

	if (package->loadClipsFromFile(fileName))
	{
		this->addClipPackage(package);

		return true;
	}
	else
	{
		delete package;
		package = nullptr;
	}
#endif

	return false;
}

void fairytale::playCustomFairytaleSlot()
{
	QAction *action = dynamic_cast<QAction*>(sender());

	CustomFairytaleActions::iterator iterator = this->m_customFairytaleActions.find(action);

	if (iterator != this->m_customFairytaleActions.end())
	{
		this->playCustomFairytale(iterator.value());
	}
}

void fairytale::addCustomFairytale(CustomFairytale *customFairytale)
{
	this->m_customFairytales.insert(customFairytale->name(), customFairytale);

	QAction *action = new QAction(customFairytale->name(), menuCustomFairytales);
	connect(action, &QAction::triggered, this, &fairytale::playCustomFairytaleSlot);
	menuCustomFairytales->addAction(action);

	m_customFairytaleActions.insert(action, customFairytale);
}

void fairytale::removeCustomFairytale(CustomFairytale *customFairytale)
{
	CustomFairytales::iterator iterator = this->m_customFairytales.find(customFairytale->name());

	if (iterator != this->m_customFairytales.end())
	{
		this->m_customFairytales.erase(iterator);

		// TODO linear time
		for (CustomFairytaleActions::iterator actionIterator = m_customFairytaleActions.begin(); actionIterator != m_customFairytaleActions.end(); ++actionIterator)
		{
			if (actionIterator.value() == customFairytale)
			{
				QAction *action = actionIterator.key();
				m_customFairytaleActions.erase(actionIterator);
				delete action;

				break;
			}
		}

		delete customFairytale;
	}
}


#include "fairytale.moc"
