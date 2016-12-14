#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtMultimedia>
#include <QTouchDevice>
#include <QMimeDatabase>
#include <QMimeType>

#include "fairytale.h"
#include "clip.h"
#include "bonusclip.h"
#include "player.h"
#include "iconbutton.h"
#include "clippackagedialog.h"
#include "clippackage.h"
#include "clippackageeditor.h"
#include "customfairytaledialog.h"
#include "gamemodeoneoutoffour.h"
#include "gamemodemoving.h"
#include "gamemodecreative.h"
#include "gamemodesequence.h"
#include "aboutdialog.h"
#include "settingsdialog.h"
#include "wondialog.h"
#include "gameoverdialog.h"
#include "highscores.h"
#include "customfairytale.h"
#include "clipeditor.h"
#include "bonusclipsdialog.h"
#include "fairytalesdialog.h"
#include "localedialog.h"
#include "config.h"

fairytale::WidgetSizes fairytale::m_widgetSizes;

QPalette fairytale::gameColorPalette()
{
	// set black background
	QPalette palette = qApp->palette();
	palette.setColor(QPalette::Background, QColor(0xCEA66B));
	palette.setColor(QPalette::Button, QColor(0xCEA69E));
	palette.setColor(QPalette::Base, QColor(0xCEA66B)); // text input background
	palette.setColor(QPalette::Highlight, QColor(0xC05800));
	palette.setColor(QPalette::Link, QColor(0xC05800));
	palette.setColor(QPalette::WindowText, Qt::black);

	palette.setColor(QPalette::Light, QColor(0xC05800)); // button selection?

	return palette;
}

QString fairytale::gameStyleSheet()
{
	return QString(
	"QComboBox, QPushButton, QMenuBar, QSpinBox, QCheckBox, QTableView, QTreeView, QHeaderView, QMessageBox { background-color: #C05800; } QMenuBar::item:selected { background: #C05800; } "
	"QFrame, QLabel, QToolTip { border: black; }"
	"QGroupBox { background-color: #C05800; margin-top: 35px; border: 2px solid black; } QGroupBox QPushButton, QGroupBox QSpinBox, QGroupBox QComboBox { background-color: #CEA66B; }"
	"QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; /* position at the left center */ padding: 0 3px; background-color: #C05800; border: 2px solid black; }"
	"QHeaderView::section { background-color: #CEA66B; }"
	"QComboBox QAbstractItemView { selection-background-color: #CEA66B; background-color: #CEA66B; border: 2px solid black; }"
	);
}

QFont fairytale::gameFont()
{
	/*
	 * Use a custom font.
	 */
	const int id = QFontDatabase::addApplicationFont(":/resources/RINGM___.TTF");
	const QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	QFont result(qApp->font());
	result.setPointSize(20);
	result.setFamily(family);

	return result;
}

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

	if (this->m_clipPackageDialog == nullptr)
	{
		this->m_clipPackageDialog = new ClipPackageDialog(this);
	}

	this->m_clipPackageDialog->fill(this->clipPackages(), this->gameModes(), this);
	ClipPackages clipPackages;
	GameMode *gameMode = nullptr;

	if (execInCentralWidgetIfNecessary(this->m_clipPackageDialog) == QDialog::Accepted)
	{
		clipPackages = this->m_clipPackageDialog->clipPackages();
		gameMode = this->m_clipPackageDialog->gameMode();
	}

	if (!clipPackages.isEmpty() && gameMode != nullptr)
	{
		startNewGame(clipPackages, gameMode, this->m_clipPackageDialog->difficulty(), this->m_clipPackageDialog->useMaxRounds(), this->m_clipPackageDialog->maxRounds());
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
	if (!this->isGameRunning() && !this->m_playCompleteSolution)
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

	execInCentralWidgetIfNecessary(this->customFairytaleDialog());

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
	execInCentralWidgetIfNecessary(settingsDialog());

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

	const bool pausedGame = this->isGameRunning() && !this->isGamePaused();

	if (pausedGame)
	{
		pauseGame();
	}

	execInCentralWidgetIfNecessary(this->m_editor);

	// continue game
	if (pausedGame)
	{
		resumeGame();
	}
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

QRect fairytale::screenRect()
{
	/*
	 * Get the screen size of the current screen where the application is shown.
	 */
	return qApp->primaryScreen()->geometry();

	// TODO react to 	primaryScreenChanged(QScreen *screen)
}

Qt::ScreenOrientation fairytale::screenOrientation()
{
	const Qt::ScreenOrientation orientation = qApp->primaryScreen()->orientation();

	if (orientation == Qt::PrimaryOrientation)
	{
		const Qt::ScreenOrientation primaryOrientation = qApp->primaryScreen()->primaryOrientation();

		return primaryOrientation;
	}

	// TODO react to orientationChanged(Qt::ScreenOrientation orientation)

	return orientation;
}

QRect fairytale::referenceRect()
{
	/*
	 * The original UI files have been designed on a system with this default size.
	 */
	return QRect(0, 0, 1920, 1080);
}

Qt::ScreenOrientation fairytale::referenceOrientation()
{
	// the width must be bigger than or equal to the height
	if (referenceRect().width() >= referenceRect().height())
	{
		return Qt::LandscapeOrientation;
	}

	return Qt::PortraitOrientation;
}

qreal fairytale::screenWidthRatio()
{
	const qreal widthRatio = (qreal)(screenRect().width()) / (qreal)(referenceRect().width());

	qDebug() << "reference width" << referenceRect().width() << "Screen width" << screenRect().width() << "Screen width ratio" << widthRatio;

	return widthRatio;
}

qreal fairytale::screenHeightRatio()
{
	const qreal heightRatio = (qreal)(screenRect().height()) / (qreal)(referenceRect().height());

	qDebug() << "reference height" << referenceRect().height() << "Screen height" << screenRect().height() << "Screen height ratio" << heightRatio;

	return heightRatio;
}

void fairytale::updateSize(QWidget *widget)
{
#if 1 == 0 //def Q_OS_ANDROID
	// TEST At the moment there is no proper way to dynamically scale widgets due to the DPI size since it might become too big or too small.
	if (dynamic_cast<QPushButton*>(widget) != nullptr)
	{
		WidgetSizes::iterator iterator = m_widgetSizes.find(widget);

		if (iterator == m_widgetSizes.end())
		{
			WidgetSizeData sizeData;
			sizeData.size = widget->size();
			sizeData.font =  widget->font();

			iterator = m_widgetSizes.insert(widget, sizeData);
		}
		else
		{
			qDebug() << "update size a second or another time for widget" << widget;
		}

		const WidgetSizeData &sizeData = iterator.value();

		/*
		 * Resize to a scaled size.
		 */
		const QSize newSize = widgetSize(sizeData.size);
		widget->resize(newSize);

		const bool usePixelSize = sizeData.font.pixelSize() != -1;
		QFont oldFont = sizeData.font;
		const int oldFontSize = usePixelSize ? oldFont.pixelSize() : oldFont.pointSize();
		const int newFontSize = fontSize(oldFontSize);

		if (usePixelSize)
		{
			oldFont.setPixelSize(newFontSize);
		}
		else
		{
			oldFont.setPointSize(newFontSize);
		}

		widget->setFont(oldFont);

		qDebug() << "Old size:" << sizeData.size << "old font size:" << oldFontSize << "new size:" << newSize << "new font size:" << newFontSize;
		//widget->
	}

	// TODO if it is horizontal or vertical layout change if the orientation changed

	if (widget->layout() != nullptr)
	{
		for (int i = 0; i < widget->layout()->count(); ++i)
		{
			QLayoutItem *item = widget->layout()->itemAt(i);

			if (item->widget() != nullptr)
			{
				updateSize(item->widget());
			}
		}
	}
#endif
}

QSize fairytale::widgetSize(const QSize &currentSize)
{
	return QSize((qreal)(currentSize.width()) * screenWidthRatio(), (qreal)(currentSize.height()) * screenHeightRatio());
}

qreal fairytale::fontSize(int currentFontSize)
{
	return (qreal)(currentFontSize) * screenWidthRatio();
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

void fairytale::startNewGame(const ClipPackages &clipPackages, GameMode *gameMode, Difficulty difficulty, bool useMaxRounds, int maxRounds)
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
	this->pauseGamePushButton->setText(tr("Pause Game (P)"));
	this->m_player->pauseButton()->setText(tr("Pause Game (P)"));

	// requires person in the first step always
	this->m_requiresPerson = true;
	this->m_turns = 0;
	this->m_totalElapsedTime = 0;
	this->scrollArea->hide();
	this->versionLabel->hide();
	this->gameAreaWidget()->show();
	this->descriptionLabel->show();
	this->timeLabel->show();

	this->m_currentClipPackages = clipPackages;
	this->m_gameMode = gameMode;
	this->m_difficulty = difficulty;
	this->m_useMaxRounds = useMaxRounds;
	this->m_maxRounds = maxRounds;
	this->gameMode()->start();
	qDebug() << "start";
	this->m_isRunning = true;

	setGameButtonsEnabled(true);

	if (!this->defaultClipPackage()->intro().isEmpty() && this->gameMode()->playIntro())
	{
		this->m_playIntro = true;
		this->m_player->playVideo(this, this->defaultClipPackage()->intro(), tr("Intro"));
	}
	else
	{
		nextTurn();
	}
}

QStringList fairytale::languages() const
{
	const QDir dir(translationsDir());

	QStringList result;

	foreach (const QFileInfo &languageFile, dir.entryInfoList(QStringList("*.qm")))
	{
		result << languageFile.baseName();
	}

	return result;
}

fairytale::fairytale(Qt::WindowFlags flags)
: QMainWindow(0, flags)
, m_turns(0)
, m_player(new Player(this, this))
, m_settingsDialog(nullptr)
, m_clipsDialog(nullptr)
, m_clipPackageDialog(nullptr)
, m_editor(nullptr)
, m_customFairytaleDialog(nullptr)
, m_remainingTime(0)
, m_requiresPerson(true)
, m_playIntro(false)
, m_playOutroWin(false)
, m_playOutroLose(false)
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
, m_difficulty(Difficulty::Normal)
, m_useMaxRounds(false)
, m_maxRounds(0)
, m_aboutDialog(nullptr)
, m_wonDialog(nullptr)
, m_gameOverDialog(nullptr)
, m_highScores(new HighScores(this, this))
, m_playingBonusClip(false)
, m_playingCustomFairytale(nullptr)
, m_customFairytaleIndex(0)
, m_pauseGameShortcut(nullptr)
, m_cancelGameShortcut(nullptr)
, m_currentScreen(nullptr)
, m_centralDialogResult(-1)
{
	this->m_player->hide();

	setupUi(this);

	this->versionLabel->setText(tr("Version: %1").arg(gustavsfairyland_VERSION));

	this->advancedGroupBox->setChecked(false);
	this->advancedGroupBoxWidget->hide();

	this->m_currentScreen = qApp->primaryScreen();
	changePrimaryScreen(this->m_currentScreen);
	connect(qApp, &QGuiApplication::primaryScreenChanged, this, &fairytale::changePrimaryScreen);

	connect(&this->m_timer, &QTimer::timeout, this, &fairytale::timerTick);

	connect(pauseGamePushButton, &QPushButton::clicked, this, &fairytale::pauseGameAction);
	connect(cancelGamePushButton, &QPushButton::clicked, this, &fairytale::cancelGame);

	connect(quickGamePushButton, &QPushButton::clicked, this, &fairytale::quickGame);
	connect(customGamePushButton, &QPushButton::clicked, this, &fairytale::newGame);
	connect(highScoresPushButton, &QPushButton::clicked, this, &fairytale::showHighScores);
	connect(recordPushButton, &QPushButton::clicked, this, &fairytale::record);

	connect(bonusClipsPushButton, &QPushButton::clicked, this, &fairytale::showBonusClipsDialog);
	connect(fairytalesPushButton, &QPushButton::clicked, this, &fairytale::showFairytalesDialog);
	connect(editorPushButton, &QPushButton::clicked, this, &fairytale::openEditor);
	connect(creditsPushButton, &QPushButton::clicked, this, &fairytale::about);

	connect(languagePushButton, &QPushButton::clicked, this, &fairytale::showLocaleDialog);
	connect(settingsPushButton, &QPushButton::clicked, this, &fairytale::settings);
	connect(quitPushButton, &QPushButton::clicked, this, &fairytale::close);

	connect(this->m_player, &Player::stateChangedVideoAndSounds, this, &fairytale::finishNarrator);

	/*
	 * NOTE: Don't use the same shortcuts for menu actions or anything else to avoid ambiguous shortcuts.
	 */
	m_pauseGameShortcut = new QShortcut(QKeySequence(tr("P")), this);
	m_pauseGameShortcut->setContext(Qt::ApplicationShortcut);
	connect(m_pauseGameShortcut, &QShortcut::activated, this, &fairytale::pauseGameAction);
	connect(m_pauseGameShortcut, &QShortcut::activatedAmbiguously, this, &fairytale::pauseGameAction); // TEST
	m_cancelGameShortcut = new QShortcut(QKeySequence(tr("C")), this);
	m_cancelGameShortcut->setContext(Qt::ApplicationShortcut);
	connect(m_cancelGameShortcut, &QShortcut::activated, this, &fairytale::cancelGame);
	connect(m_cancelGameShortcut, &QShortcut::activatedAmbiguously, this, &fairytale::cancelGame); // TEST

	m_audioPlayer->setAudioRole(QAudio::GameRole);
	m_audioPlayer->setVolume(defaultClickSoundsVolume);
	connect(this->m_audioPlayer, &QMediaPlayer::stateChanged, this, &fairytale::finishAudio);

	m_musicPlayer->setAudioRole(QAudio::GameRole);
	connect(this->m_musicPlayer, &QMediaPlayer::stateChanged, this, &fairytale::finishMusic);

	GameModeMoving *gameModeMoving = new GameModeMoving(this);
	m_gameModes.insert(gameModeMoving->id(), gameModeMoving);
	GameModeOneOutOfFour *gameModeOneOutOfFour = new GameModeOneOutOfFour(this);
	m_gameModes.insert(gameModeOneOutOfFour->id(), gameModeOneOutOfFour);
	GameModeCreative *gameModeCreative = new GameModeCreative(this);
	m_gameModes.insert(gameModeCreative->id(), gameModeCreative);
	GameModeSequence *gameModeSequence = new GameModeSequence(this);
	m_gameModes.insert(gameModeSequence->id(), gameModeSequence);

	QSettings settings("TaCaProduction", "gustavsfairyland");

	// Loads the clips directory and applies it. Besides loads the clip packages and sets the default clip package if none is found.
	settingsDialog()->load(settings);

	const int highScoresSize = settings.beginReadArray("highscores");
	qDebug() << "Read high scores:" << highScoresSize;

	for (int i = 0; i < highScoresSize; ++i)
	{
		settings.setArrayIndex(i);
		HighScore highScore(settings.value("name").toString(), settings.value("packages").toString().split(";"), settings.value("gameMode").toString(), (fairytale::Difficulty)settings.value("difficulty").toInt(), settings.value("rounds").toInt(), settings.value("time").toInt());
		this->highScores()->addHighScore(highScore);
	}

	settings.endArray();

	const int bonusClipUnlocksSize = settings.beginReadArray("bonusclipunlocks");

	for (int i = 0; i < bonusClipUnlocksSize; ++i)
	{
		settings.setArrayIndex(i);
		const QString packageId = settings.value("package").toString();
		const QString clipId = settings.value("clip").toString();

		this->m_bonusClipUnlocks.insert(ClipKey(packageId, clipId));
	}

	settings.endArray();

	// searched in reverse order
	qApp->installTranslator(&m_qtTranslator);
	qApp->installTranslator(&m_qtBaseTranslator);
	qApp->installTranslator(&m_translator);

	// Try to load the current locale. If no translation file exists it will remain English.
	const QString locale = systemLocale();
	loadLanguage(locale);

	// Initial cleanup.
	cleanupGame();

	// start the background music from Mahler
	startMusic();

	/*
	 * Make sure that at least the default clip packages are there.
	 * If no clip packages have been loaded, the clip path is probably different from a previous installation.
	 * Therefore reset to the default clips directory and try to load the default clip packages.
	 *
	 * If the clips directory is different from the default dir ask for resetting it to make it easier for normal players
	 * to use the latest clip files.
	 */
	if (this->m_clipPackages.isEmpty())
	{
		resetToDefaultClipPackages();
	}
	// Make sure both return absolute paths.
	else if (this->defaultClipsDirectory() != this->clipsDir().toLocalFile())
	{
		if (QMessageBox::question(this, tr("Use default clip packages?"), tr("Your clip package directory has been changed. This might be due to an old installation of this game. Some of the latest clips might not be available. Do you want to use the default clip packages of the current installation?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape) == QMessageBox::Yes)
		{
			resetToDefaultClipPackages();
		}
	}

	qDebug() << "Compared clips dirs:" << this->defaultClipsDirectory() << "and" << this->clipsDir().toLocalFile();
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
			settings.setValue("packages", highScore.packages().join(";"));
			settings.setValue("gameMode", highScore.gameMode());
			settings.setValue("difficulty", (int)highScore.difficulty());
			settings.setValue("rounds", highScore.rounds());
			settings.setValue("time", highScore.time());

			++i;
		}
	}

	qDebug() << "Wrote high scores:" << i;

	settings.endArray();

	settings.beginWriteArray("bonusclipunlocks", this->m_bonusClipUnlocks.size());
	i = 0;

	for (BonusClipUnlocks::const_iterator iterator = this->m_bonusClipUnlocks.begin(); iterator != this->m_bonusClipUnlocks.end(); ++iterator, ++i)
	{
		settings.setArrayIndex(i);
		const QString packageId = iterator->first;
		settings.setValue("package", packageId);
		const QString clipId = iterator->second;
		settings.setValue("clip", clipId);
	}

	settings.endArray();

	qApp->removeTranslator(&m_translator);
	qApp->removeTranslator(&m_qtBaseTranslator);
	qApp->removeTranslator(&m_qtTranslator);

	// Prevent restarting on ending the app.
	disconnect(this->m_musicPlayer, &QMediaPlayer::stateChanged, this, &fairytale::finishMusic);
}

QString fairytale::defaultClipsDirectory() const
{
#ifdef DEBUG
	/*
	 * In debug mode we are in the directory "build", so we have to use a different relative path.
	 */
	const QDir dir(QCoreApplication::applicationDirPath() + "/../clips");

	qDebug() << "Absolute dir path" << dir.absolutePath();

	return dir.absolutePath(); // The absolute path is required for comparison at start since Settings uses the absolute path as well.
#elif defined(Q_OS_WIN)
	/*
	 * The player usually starts the binary via a Desktop link which runs it in the directory:
	 * "C:\Program Files (x86)\gustavsfairyland"
	 * But the player can also directly start the .exe file in the bin folder.
	 * Therefore one must get the file path to the binary file and not use QDir::currentPath().
	 */
	const QDir dir(QCoreApplication::applicationDirPath() + "/../share/gustavsfairyland/clips");

	qDebug() << "Absolute dir path" << dir.absolutePath();

	return dir.absolutePath(); // The absolute path is required for comparison at start since Settings uses the absolute path as well.
#elif defined(Q_OS_ANDROID)
	/*
	 * Android uses the assets protocol for files which are deployed with an app.
	 */
	return QString("assets:/clips");
#else
	return QString("/usr/share/gustavsfairyland/clips");
#endif
}

bool fairytale::ensureCustomClipsExistence()
{
	const QDir customClipsDir = QDir(customClipsDirectory());

	if (!customClipsDir.exists())
	{
		const QDir dataLocation = customClipsParentDirectory();

		qDebug() << "Creating \"customclips\" dir in" << dataLocation.absolutePath();

		if (!dataLocation.mkpath("customclips"))
		{
			qDebug() << "Error on creating custom.xml dir";

			return false;
		}
	}
	else
	{
		qDebug() << "Custom clips dir exists:" << customClipsDir.absolutePath();
	}

	const QFileInfo currentCustomFileInfo(customClipsDir.filePath("custom.xml"));

	/*
	 * If the custom file does not exist in the home directory, copy the default file there.
	 */
	if (!currentCustomFileInfo.exists())
	{
		const QFileInfo customFileInfo(this->defaultClipsDirectory() + "/custom.xml");

		if (!customFileInfo.exists())
		{
			qDebug() << "Custom clips file does not exist:" << customFileInfo.absoluteFilePath();

			return false;
		}

		if (!QFile::copy(customFileInfo.absoluteFilePath(), currentCustomFileInfo.absoluteFilePath()))
		{
			qDebug() << "Error on copying custom.xml file from" << customFileInfo.absoluteFilePath() << "to" << currentCustomFileInfo.absoluteFilePath();

			return false;
		}
	}

	return true;
}

QString fairytale::customClipsParentDirectory() const
{
	const QDir dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	return dataLocation.absolutePath();
}

QString fairytale::customClipsDirectory() const
{
	const QString subDir = "customclips";
	const QDir dataLocation = customClipsParentDirectory();
	const QDir customClipsDir = dataLocation.filePath(subDir);

	return customClipsDir.absolutePath();
}

void fairytale::playFinalClip(int index)
{
	if (index >= this->m_completeSolution.size() || index < 0)
	{
		qDebug() << "Invalid final clip index" << index;

		return;
	}

	this->m_completeSolutionIndex = index;
	fairytale::ClipKey solution = this->m_completeSolution[index];
	Clip *solutionClip = this->getClipByKey(solution);
	Clip *startPersonClip = this->getClipByKey(m_startPerson);

	// play all sounds parallel to the clip

	// play the sound for the inital character again
	if (solutionClip->isPerson() && index > 1)
	{
		this->m_player->playParallelSound(this, this->resolveClipUrl(startPersonClip->narratorUrl()));
	}

	// play the sound "and"
	if (solutionClip->isPerson() && index > 0)
	{
		this->m_player->playParallelSound(this, this->narratorSoundUrl());
	}

	this->m_player->playParallelSound(this, this->resolveClipUrl(solutionClip->narratorUrl()));
	this->m_player->playVideo(this, solutionClip->videoUrl(), this->description(startPersonClip, index, solutionClip));
}

void fairytale::playFinalVideo()
{
	if (m_playCompleteSolution)
	{
		qDebug() << "Plays already the complete solution.";

		return;
	}

	this->m_playCompleteSolution = true;
	this->playFinalClip(0);
}

void fairytale::playCustomFairytaleClip(int index)
{
	this->m_customFairytaleIndex = index;
	const QString packageId = this->m_playingCustomFairytale->clipIds()[index].first;

	bool played = false;
	ClipPackages::iterator iterator = this->m_clipPackages.find(packageId);

	if (iterator != this->m_clipPackages.end())
	{
		ClipPackage *clipPackage = iterator.value();
		const QString clipId = this->m_playingCustomFairytale->clipIds()[index].second;
		ClipPackage::Clips::iterator clipIterator = clipPackage->clips().find(clipId);

		if (clipIterator != clipPackage->clips().end())
		{
			Clip *solution = clipIterator.value();
			Clip *startPersonClip = solution;

			const QString startPersonPackageId = this->m_playingCustomFairytale->clipIds()[0].first;
			ClipPackages::iterator startPersonPackageIterator = this->m_clipPackages.find(packageId);

			if (startPersonPackageIterator != this->m_clipPackages.end())
			{
				ClipPackage *startPersonClipPackage = iterator.value();
				const QString startPersonClipId = this->m_playingCustomFairytale->clipIds()[0].second;
				ClipPackage::Clips::iterator startPersonClipIterator = startPersonClipPackage->clips().find(startPersonClipId);

				if (startPersonClipIterator != startPersonClipPackage->clips().end())
				{
					startPersonClip = startPersonClipIterator.value();
				}


				played = true;

				// play all sounds parallel to the clip

				// play the sound for the inital character again
				if (solution->isPerson() && index > 1)
				{
					this->m_player->playParallelSound(this, this->resolveClipUrl(startPersonClip->narratorUrl()));
				}

				// play the sound "and"
				if (solution->isPerson() && index > 0)
				{
					this->m_player->playParallelSound(this, this->narratorSoundUrl());
				}

				this->m_player->playParallelSound(this, this->resolveClipUrl(solution->narratorUrl()));
				this->m_player->playVideo(this, solution->videoUrl(), this->description(startPersonClip, index, solution), false);
			}
		}
	}

	if (!played)
	{
		finishPlayingCustomFairytale();
	}
}

void fairytale::playCustomFairytale(CustomFairytale *customFairytale)
{
	if (m_playingCustomFairytale != nullptr || this->isGameRunning() || this->m_playCompleteSolution)
	{
		qDebug() << "Invalid playCustomFairytale() call";

		return;
	}

	if (!customFairytale->clipIds().isEmpty())
	{
		this->m_playingCustomFairytale = customFairytale;
		this->playCustomFairytaleClip(0);
	}
}

fairytale::Widgets fairytale::hideWidgetsInMainWindow()
{
	// TODO make a more generic way to store all shown widgets and hide them and show them afterwards
	QStack<QWidget*> widgets;
	widgets.push(this->centralWidget());
	Widgets hiddenWidgets;

	while (!widgets.isEmpty())
	{
		QWidget *widget = widgets.pop();
		qDebug() << "Checking widget" << widget;

		if (widget->isVisible())
		{
			// Do never hide the central widget itself.
			if (widget != this->centralWidget())
			{
				qDebug() << "Hiding widget " << widget->objectName();
				hiddenWidgets.push_back(widget);
				widget->hide();
			}

			if (widget->layout() != nullptr)
			{
				for (int i = 0; i < widget->layout()->count(); ++i)
				{
					QWidget *layoutItemWidget = widget->layout()->itemAt(i)->widget();

					if (layoutItemWidget != nullptr)
					{
						widgets.push(layoutItemWidget);
					}
				}
			}
		}
	}

	return hiddenWidgets;
}

void fairytale::showWidgetsInMainWindow(Widgets widgets)
{
	foreach (QWidget *widget, widgets)
	{
		widget->show();
	}
}

int fairytale::execInCentralWidgetIfNecessaryEx(QDialog *dialog, std::function<void(QDialog*)> lambda)
{
	// TODO disable and enable all menu bar actions as well as long as the widget is shown
	const Widgets hiddenWidgets = hideWidgetsInMainWindow();

	const bool wasModal = dialog->isModal();
	dialog->setModal(false);
	this->centralWidget()->layout()->addWidget(dialog);
	/*
	 * Place the dialog widget at the center of the central widget.
	 * This does also align widgets properly which are too small.
	 */
	QGridLayout *gridLayout = dynamic_cast<QGridLayout*>(this->centralWidget()->layout());
	/*
	 * Take all space which is available. Don't use Qt::AlignCenter. Otherwise it won't use all space.
	 */
	gridLayout->addWidget(dialog, 0, 0, -1, -1);
	dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->centralWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	/*
	 * exec() cannot be used since it makes the dialog modal and on Linux the main window is not updated properly anymore.
	 * Therefore the result of the dialog has to be waited for and stored manually.
	 */
	m_centralDialogResult = -1;

	connect(dialog, &QDialog::finished, this, &fairytale::finishCentralDialog);
	dialog->show();

	/*
	 * Might execute several other central widget dialogs recursively.
	 */
	lambda(dialog);

	/*
	 * This event loop is usually done by dialog->exec() but since we don't want to use exec() it has to be emulated.
	 * It waits until the result of the dialog has been specified and handles all events during that time.
	 * The check is done via polling.
	 */
	QEventLoop eventLoop(this);

	while (m_centralDialogResult == -1)
	{
		eventLoop.processEvents(QEventLoop::AllEvents, 1000);
	}

	qDebug() << "Result is " << m_centralDialogResult << " for widget " << dialog->objectName();
	eventLoop.quit();

	const int result = m_centralDialogResult;
	m_centralDialogResult = -1;

	this->centralWidget()->layout()->removeWidget(dialog);
	dialog->setModal(wasModal);

	showWidgetsInMainWindow(hiddenWidgets);

	return result;
}

int fairytale::execInCentralWidgetIfNecessary(QDialog* dialog)
{
	return execInCentralWidgetIfNecessaryEx(dialog, [](QDialog *dialog) { });
}

ClipPackage* fairytale::getClipPackageById(const QString &packageId)
{
	ClipPackages::iterator iterator = this->m_clipPackages.find(packageId);

	if (iterator != this->m_clipPackages.end())
	{
		return iterator.value();
	}

	return nullptr;
}

Clip* fairytale::getClipByKey(const ClipKey &clipKey)
{
	ClipPackage *clipPackage = getClipPackageById(clipKey.first);

	if (clipPackage == nullptr)
	{
		return nullptr;
	}

	ClipPackage::Clips::iterator iterator = clipPackage->clips().find(clipKey.second);

	if (iterator != clipPackage->clips().end())
	{
		return iterator.value();
	}

	return nullptr;
}

int fairytale::maxRoundsByMultipleClipPackages(const ClipPackages &clipPackages)
{
	int actClips = 0;
	int personClips = 0;

	for (ClipPackages::const_iterator packageIterator = clipPackages.begin(); packageIterator != clipPackages.end(); ++packageIterator)
	{
		const ClipPackage::Clips &clips = packageIterator.value()->clips();

		for (ClipPackage::Clips::const_iterator clipIterator = clips.begin(); clipIterator != clips.end(); ++clipIterator)
		{
			if (clipIterator.value()->isPerson())
			{
				++personClips;
			}
			else
			{
				++actClips;
			}
		}
	}

	if (personClips == 0)
	{
		return 0;
	}

	return qMin(personClips - 1, actClips);
}

QAudioOutputSelectorControl* fairytale::audioOutputSelectorControl() const
{
	QMediaService *svc = m_musicPlayer->service();

	if (svc != nullptr)
	{
		QAudioOutputSelectorControl *out = reinterpret_cast<QAudioOutputSelectorControl*>(svc->requestControl(QAudioOutputSelectorControl_iid));

		qDebug() << "Audio output selector control " << out;

		if (out != nullptr)
		{
			qDebug() << "Audio output selector control default output " << out->defaultOutput();

			return out;
		}
	}

	return nullptr;
}

QAudioInputSelectorControl* fairytale::audioInputSelectorControl() const
{
	QMediaService *svc = m_musicPlayer->service();

	if (svc != nullptr)
	{
		QAudioInputSelectorControl *in = reinterpret_cast<QAudioInputSelectorControl*>(svc->requestControl(QAudioInputSelectorControl_iid));

		qDebug() << "Audio input selector control " << in;

		if (in != nullptr)
		{
			qDebug() << "Audio input selector control default output " << in->defaultInput();

			return in;
		}
	}

	return nullptr;
}

void fairytale::showHighScores()
{
	const bool pausedGame = this->isGameRunning() && !this->isGamePaused();

	if (pausedGame)
	{
		pauseGame();
	}

	execInCentralWidgetIfNecessary(this->highScores());

	// continue game
	if (pausedGame)
	{
		resumeGame();
	}
}

void fairytale::record()
{
	const bool pausedGame = this->isGameRunning() && !this->isGamePaused();

	if (pausedGame)
	{
		pauseGame();
	}

	if (customClipPackage() != nullptr)
	{
		Clip clip(this);
		/*
		 * Don't use any special characters which are required for custom fairytale strings.
		 */
		const QString id = QDateTime::currentDateTime().toString(Qt::ISODate).remove(QRegExp("[;:]"));
		clip.setId(id);
		ClipEditor clipEditor(this, this);
		clipEditor.fill(&clip);

		if (ensureCustomClipsExistence())
		{
			clipEditor.setTargetClipsDirectory(QDir(customClipsDirectory()));

			if (execInCentralWidgetIfNecessaryEx(&clipEditor, [](QDialog *dialog) {
				/*
				 * Let the user record a video, a narrating sound and capture an image immediately.
				 * This makes it much faster to create custom clips.
				 */
				ClipEditor *clipEditor = dynamic_cast<ClipEditor*>(dialog);

				if (clipEditor->recordVideo() == QDialog::Rejected)
				{
					return;
				}

				if (clipEditor->recordNarratingSoundSimple() == QDialog::Rejected)
				{
					return;
				}

				if (clipEditor->captureImage() == QDialog::Rejected)
				{
					return;
				}
			}) == QDialog::Accepted)
			{
				Clip *clipOfCustomPackage = clipEditor.clip(this->customClipPackage());
				this->customClipPackage()->addClip(clipOfCustomPackage);

				/*
				* The custom clips file has to be in the home directory to be writable and unique for the user.
				*/
				if (!this->customClipPackage()->saveClipsToFile())
				{
					QMessageBox::critical(this, tr("Error on Saving Custom Clips Package"), tr("Error on Saving Custom Clips Package."));
				}
			}
		}
		else
		{
			QMessageBox::critical(this, tr("Missing Custom Clip Dir"), tr("Custom Clip Dir is missing."));
		}
	}
	else
	{
		QMessageBox::critical(this, tr("Missing Custom Clip Package"), tr("Custom Clip Package is missing."));
	}

	// continue game
	if (pausedGame)
	{
		resumeGame();
	}
}

void fairytale::about()
{
	const bool pausedGame = this->isGameRunning() && !this->isGamePaused();

	if (pausedGame)
	{
		pauseGame();
	}

	execInCentralWidgetIfNecessary(this->aboutDialog());

	// continue game
	if (pausedGame)
	{
		resumeGame();
	}
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
	ClipPackages clipPackages = this->defaultClipPackages();

	if (this->maxRoundsByMultipleClipPackages(clipPackages) <= 0)
	{
		QMessageBox::critical(this, tr("Unable to start the game"), tr("Unable to start the game since clips are missing. Try to restore the default settings."));

		return;
	}

	GameMode *gameMode = this->defaultGameMode();
	const Difficulty difficulty = this->defaultDifficulty();
	const bool useMaxRounds = this->defaultUseMaxRounds();
	const int maxRounds = this->defaultMaxRounds();

	startNewGame(clipPackages, gameMode, difficulty, useMaxRounds, maxRounds);
}

void fairytale::retry(GameMode *gameMode, Difficulty difficulty)
{
	if (this->clipPackages().isEmpty() || this->gameModes().isEmpty())
	{
		return;
	}

	// Start with the first available stuff.
	ClipPackages clipPackages = this->currentClipPackages();
	const bool useMaxRounds = this->useMaxRounds();
	const int maxRounds = this->maxRounds();

	this->startNewGame(clipPackages, gameMode, difficulty, useMaxRounds, maxRounds);
}

QString fairytale::systemLocale()
{
	// Try to load the current locale. If no translation file exists it will remain English.
	QString locale = QLocale::system().name();
	locale.truncate(locale.lastIndexOf('_'));

	return locale;
}

QDir fairytale::translationsDir() const
{
#if defined(DEBUG) && not defined(Q_OS_ANDROID)
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

bool fairytale::loadLanguage(const QString &language)
{
	bool result = false;
	const QString translationsDirPath = translationsDir().path();
	const QString fileName = language + ".qm";
	qDebug() << "Translation directory: " << translationsDirPath;
	qDebug() << "Translation: " << fileName;

	if (m_translator.load(fileName, translationsDirPath))
	{
		qDebug() << "Loaded file!";
		qDebug() << "File loaded:" << fileName;

		result = true;
	}
	else
	{
		qDebug() << "Did not load file: " << fileName << " from dir " << translationsDirPath;
		qWarning() << "File not loaded";
	}

	/*
	 * Load Qt translations as well:
	 * http://stackoverflow.com/a/31557808
	 * On Fedora this requires the package qt5-qtranslations!
	 */
	const QString qtTranslationsDirPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);

	if (m_qtTranslator.load(QString("qt_") + language, qtTranslationsDirPath))
	{
		qDebug() << "Loaded Qt file!";
		qDebug() << "Qt File loaded:" << language;
	}
	else
	{
		qDebug() << "Did not load file: " << language << " from dir " << qtTranslationsDirPath;
		qWarning() << "File not loaded";
	}

	if (m_qtBaseTranslator.load(QString("qtbase_") + language, qtTranslationsDirPath))
	{
		qDebug() << "Loaded Qt base file!";
		qDebug() << "Qt Base File loaded:" << QString("qtbase_") + language;
	}
	else
	{
		qDebug() << "Did not load file: " << QString("qtbase_") + language << " from dir " << qtTranslationsDirPath;
		qWarning() << "File not loaded";
	}

	// Do always update since the file might be empty (for English).
	m_currentTranslation = language;
	qDebug() << "Current translation:" << language;

	return result;
}

void fairytale::gameOver()
{
	this->gameMode()->end();
	this->m_isRunning = false;

	// make sure execInCentralWidgetIfNecessary() has not to hide them
	hideGameWidgets();

	const QUrl outroUrl = this->defaultClipPackage()->outros().size() > (int)ClipPackage::Outro::Lost ? this->resolveClipUrl(this->defaultClipPackage()->outros().at((int)ClipPackage::Outro::Lost)) : QUrl();

	qDebug() << "Outro URL:" << outroUrl;

	if (!outroUrl.isEmpty() && this->gameMode()->playOutro())
	{
		this->m_playOutroLose = true;
		this->m_player->playVideo(this, outroUrl, tr("Outro"));
	}
	else
	{
		this->afterOutroGameOver();
	}
}

void fairytale::afterOutroGameOver()
{
	execInCentralWidgetIfNecessary(this->gameOverDialog());

	// Show the custom fairytale dialog which allows the loser to watch his created fairytale.
	execInCentralWidgetIfNecessary(this->customFairytaleDialog());

	// dont clean up on retry
	// make sure everything is cleaned up
	if (!this->customFairytaleDialog()->clickedRetry())
	{
		this->cleanupAfterOneGame();
	}
	// prevents recursive calls in dialog
	else
	{
		this->retry(this->customFairytaleDialog()->retryGameMode(), this->customFairytaleDialog()->retryDifficulty());
	}
}

void fairytale::win()
{
	this->gameMode()->end();
	this->m_isRunning = false;

	hideGameWidgets();

	const QUrl outroUrl = this->defaultClipPackage()->outros().size() > (int)this->difficulty() ? this->resolveClipUrl(this->defaultClipPackage()->outros().at((int)this->difficulty())) : QUrl();

	qDebug() << "Outro URL:" << outroUrl;

	if (!outroUrl.isEmpty() && this->gameMode()->playOutro())
	{
		this->m_playOutroWin = true;
		this->m_player->playVideo(this, outroUrl, tr("Outro"));
	}
	else
	{
		this->afterOutroWin();
	}
}

void fairytale::afterOutroWin()
{
	if (this->gameMode()->showWinDialog())
	{
		execInCentralWidgetIfNecessary(this->wonDialog());
	}

	if (this->gameMode()->unlockBonusClip())
	{
		QSet<ClipKey> lockedBonusClips;

		for (ClipPackages::const_iterator packageIterator = this->currentClipPackages().begin(); packageIterator != this->currentClipPackages().end(); ++packageIterator)
		{
			ClipPackage *clipPackage = packageIterator.value();

			for (ClipPackage::BonusClips::const_iterator iterator = clipPackage->bonusClips().begin(); iterator != clipPackage->bonusClips().end(); ++iterator)
			{
				const ClipKey clipKey = ClipKey(clipPackage->id(), iterator.key());

				if (this->m_bonusClipUnlocks.find(clipKey) == this->m_bonusClipUnlocks.end())
				{
					lockedBonusClips.insert(clipKey);
				}
			}
		}

		if (!lockedBonusClips.isEmpty())
		{
			const int index = qrand() % lockedBonusClips.size();
			int i = 0;
			BonusClip *bonusClip = nullptr;
			ClipKey bonusClipKey;

			for (QSet<ClipKey>::iterator iterator = lockedBonusClips.begin(); iterator != lockedBonusClips.end(); ++iterator, ++i)
			{
				if (i == index)
				{
					bonusClipKey = *iterator;
					this->m_bonusClipUnlocks.insert(bonusClipKey);
					bonusClip = getBonusClipByKey(bonusClipKey);

					break;
				}
			}

			// Play the bonus clip
			if (bonusClip != nullptr)
			{
				// TODO play it
				QMessageBox::information(this, tr("Unlocked Bonus Clip!"), tr("Unlocked Bonus clip %1!").arg(bonusClip->description()));
			}
		}
		else
		{
			QMessageBox::information(this, tr("Unlocked all Bonus Clips already!"), tr("You have already unlocked all available bonus clips!"));
		}
	}

	if (this->gameMode()->addToHighScores())
	{
		QString name = qgetenv("USER");

		if (name.isEmpty())
		{
			name = qgetenv("USERNAME");
		}

		const HighScore highScore(name, this->currentClipPackages().keys(), this->gameMode()->id(), this->difficulty(), this->rounds(), this->m_totalElapsedTime);
		this->highScores()->addHighScore(highScore);
	}

	// Show the custom fairytale dialog which allows the winner to watch his created fairytale.
	execInCentralWidgetIfNecessary(this->customFairytaleDialog());

	// dont clean up on retry
	// make sure everything is cleaned up
	if (!this->customFairytaleDialog()->clickedRetry())
	{
		this->cleanupAfterOneGame();
	}
	// prevents recursive calls in dialog
	else
	{
		this->retry(this->customFairytaleDialog()->retryGameMode(), this->customFairytaleDialog()->retryDifficulty());
	}
}

bool fairytale::isGamePaused() const
{
	return this->m_paused;
}

void fairytale::pauseGame()
{
	if (this->isGamePaused() || (!this->isGameRunning() && !this->m_playCompleteSolution))
	{
		qDebug() << "Warning: Invalid call of pauseGame()";

		return;
	}

	this->pauseGamePushButton->setText(tr("Continue Game (P)"));
	this->m_player->pauseButton()->setText(tr("Continue Game (P)"));
	this->m_paused = true;

	if (this->isMediaPlayerPlaying())
	{
		this->m_player->pause();
	}
	// Dont pause the timer and game mode if the complete solution is played!
	else if (this->isGameRunning())
	{
		this->pauseTimer();
		this->gameMode()->pause();
	}
}

void fairytale::resumeGame()
{
	if (!this->isGamePaused() || (!this->isGameRunning() && !this->m_playCompleteSolution))
	{
		qDebug() << "Warning: Invalid call of resumeGame()";

		return;
	}

	this->pauseGamePushButton->setText(tr("Pause Game (P)"));
	this->m_player->pauseButton()->setText(tr("Pause Game (P)"));
	this->m_paused = false;

	if (this->isMediaPlayerPaused())
	{
		this->m_player->play();
	}
	// Dont resume the timer and game mode if the complete solution is played!
	else if (this->isGameRunning())
	{
		if (this->gameMode()->hasLimitedTime())
		{
			this->resumeTimer();
		}

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
	if (this->isTimerPaused() || !this->isTimerRunning())
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
	if (!this->isTimerPaused() || this->isTimerRunning())
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
			if (this->gameMode()->hasToChooseTheSolution())
			{
				const ClipKey clipKey = this->gameMode()->solutions().front();

				if (!clipKey.isEmpty())
				{
					Clip *solution = this->getClipByKey(clipKey);

					if (this->m_turns == 0)
					{
						this->m_startPerson = clipKey;
					}

					Clip *startPersonClip = this->getClipByKey(this->m_startPerson);
					const QString description = this->description(startPersonClip, this->m_turns, solution);

					this->descriptionLabel->clear();
					this->timeLabel->clear();

					qDebug() << "Queue the sounds.";

					// play the sound for the inital character again
					if (solution->isPerson() && turns() > 1)
					{
						PlayerSoundData data;
						data.narratorSoundUrl = startPersonClip->narratorUrl();
						data.description = this->description(startPersonClip, 0, startPersonClip);
						data.imageUrl = startPersonClip->imageUrl();
						data.prefix = true;
						this->queuePlayerSound(data);
					}

					// play the sound "and"
					if (solution->isPerson() && turns() > 0)
					{
						PlayerSoundData data;
						data.narratorSoundUrl = this->narratorSoundUrl();
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
					data.description = this->description(startPersonClip, 0, solution); // use always the stand alone description
					data.imageUrl = solution->imageUrl();
					data.prefix = false;
					this->queuePlayerSound(data);
				}
				// Some game modes dont have a single solution every turn.
				else
				{
					this->afterNarrator();
				}
			}
			// Just continue with the game mode
			else
			{
				this->afterNarrator();
			}

			break;
		}

		case GameMode::State::None:
		{
			QMessageBox::warning(this, tr("Invalid gamemode"), tr("The gamemode has invalid behaviour."));

			break;
		}
	}
}

void fairytale::onFinishTurn()
{
	// Store the time it took to finish the turn, ignore pauses!
	if (this->gameMode()->hasLimitedTime())
	{
		this->m_totalElapsedTime += this->gameMode()->time() - m_remainingTime;
		this->m_timer.stop();
	}

	this->m_remainingTime = 0;
	this->m_turns++;

	// If no start person has been selected yet, make sure this one is used as the start person.
	if (this->m_turns == 1)
	{
		this->m_startPerson = this->gameMode()->solutions().front();
	}

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
			addCurrentSolution();

			this->nextTurn();

			break;
		}

		case GameMode::State::None:
		{
			QMessageBox::warning(this, tr("Invalid gamemode"), tr("The gamemode has invalid behaviour."));

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
	m_pauseGameShortcut->setEnabled(enabled);
	m_cancelGameShortcut->setEnabled(enabled);
	gameButtonsWidget->setEnabled(enabled);
	gameButtonsWidget->setVisible(enabled);
}

void fairytale::finishNarrator(QMediaPlayer::State state)
{
	qDebug() << "Finish narrator with state:" << state;

	switch (state)
	{
		case QMediaPlayer::StoppedState:
		{
			onFinishVideoAndSounds();

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

		// played intro
		if (this->m_playIntro)
		{
			this->m_playIntro = false;
			this->m_player->stop();
			this->m_player->hide(); // hide the player, otherwise one cannot play the game
			nextTurn();
		}
		// played outro win
		else if (this->m_playOutroWin)
		{
			this->m_playOutroWin = false;
			this->m_player->stop();
			this->m_player->hide(); // hide the player, otherwise one cannot play the game
			afterOutroWin();
		}
		// played outro lose
		else if (this->m_playOutroLose)
		{
			this->m_playOutroLose = false;
			this->m_player->stop();
			this->m_player->hide(); // hide the player, otherwise one cannot play the game
			afterOutroGameOver();
		}
		// played narrator stuff
		else if (!this->m_playCompleteSolution && m_playingCustomFairytale == nullptr)
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

					this->afterNarrator();
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
			if (this->m_completeSolutionIndex + 1 < this->m_completeSolution.size() && !this->m_player->skippedAll())
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
			if (this->m_customFairytaleIndex + 1 < this->m_playingCustomFairytale->clipIds().size() && !this->m_player->skippedAll())
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
				// The dialog has to disappear, after the player watched all final clips.
				this->m_player->hide();

				this->finishPlayingCustomFairytale();
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

void fairytale::afterNarrator()
{
	this->updateTimeLabel();
	// the description label helps to remember
	this->descriptionLabel->setText(this->description(this->getClipByKey(this->m_startPerson), this->m_turns, this->getClipByKey(this->gameMode()->solutions().front())));

	this->gameMode()->afterNarrator();

	// run every second
	if (this->gameMode()->hasLimitedTime())
	{
		this->m_remainingTime = this->gameMode()->time();
		this->m_isRunningTimer = true;
		this->m_pausedTimer = false;
		this->updateTimeLabel();
		this->m_timer.start(1000);
	}
	else
	{
		this->m_isRunningTimer = false;
		this->m_pausedTimer = false;
	}
}

void fairytale::startMusic()
{
	// TODO add to package XML file, each package can have its own background music
	QList<QUrl> urls;
	urls.push_back(QUrl("./music/01.PSO020103-Mahler-5-I.mp3"));
	const QUrl url = urls.front(); //urls.at(qrand() % urls.size());
	const QUrl musicUrl = this->resolveClipUrl(url);
	qDebug() << "Play music:" << musicUrl;
	m_musicPlayer->setMedia(musicUrl);
	m_musicPlayer->play();
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
	for (ClipKey clipKey : this->gameMode()->solutions())
	{
		this->customFairytaleDialog()->addClip(clipKey);
		this->m_completeSolution.push_back(clipKey);
	}
}

QString fairytale::description(Clip *startPersonClip, int turn, Clip *clip, bool markBold)
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
				description = tr("%1 and <b>%2</b>").arg(startPersonClip->description()).arg(clip->description());
			}
			else
			{
				description = tr("%1 and %2").arg(startPersonClip->description()).arg(clip->description());
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
		qDebug() << "Play sound" << url << "because none is playing";

		m_playNewSound = false;
		m_audioPlayer->setMedia(url);
		m_audioPlayer->play();

		return true;
	}

	qDebug() << "Dont play sound" << url << "because one is playing";

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

void fairytale::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI";
			this->retranslateUi(this);
			this->updatePixmap();
			this->versionLabel->setText(tr("Version: %1").arg(gustavsfairyland_VERSION));

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

void fairytale::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	this->updateSize(scrollArea);
	this->updateSize(gameButtonsWidget);
	this->updatePixmap();
}

void fairytale::cleanupGame()
{
	this->m_playerSounds.clear();

	// this method is also called in the beginning when there is no gamemode
	if (this->gameMode() != nullptr)
	{
		this->gameMode()->end(); // end the game mode before stopping the player, the player has to know that the game mode is ended
	}

	// cleanup all flags, make sure nothing is playing when the player is stopped!
	this->m_isRunning = false;
	this->m_isPlayingMediaPlayer = false;
	this->m_pausedMediaPlayer = false;
	this->m_playIntro = false;
	this->m_playOutroWin = false;
	this->m_playOutroLose = false;

	/*
	 * Could also happen during playing the custom fairytale!
	 */
	this->m_playCompleteSolution = false;
	this->m_completeSolutionIndex = 0;
	this->customFairytaleDialog()->hide();

	// Note: Make sure this has no effect in its connected slots!
	this->m_player->stop();
	this->m_player->hide();
	this->cleanupAfterOneGame();
}

void fairytale::cleanupAfterOneGame()
{
	this->clearSolution();
	qDebug() << "Before disabling game buttons";
	this->setGameButtonsEnabled(false);
	qDebug() << "After disabling game buttons";

	hideGameWidgets();

	this->scrollArea->show();
	this->versionLabel->show();
	// Make sure all paint stuff from the game mode disappears.
	this->repaint();
}

void fairytale::hideGameWidgets()
{
	this->timeLabel->setText("");
	this->timeLabel->hide();
	this->descriptionLabel->setText("");
	this->descriptionLabel->hide();
	this->gameAreaWidget()->hide();
}

void fairytale::finishPlayingCustomFairytale()
{
	qDebug() << "Finished custom fairytale clips";
	this->m_playingCustomFairytale = nullptr;
	this->m_customFairytaleIndex = 0;
}

QUrl fairytale::narratorSoundUrl() const
{
	const QString language = this->currentTranslation();
	const QUrl narratorSoundUrl = QUrl(QString("qrc:/resources/and_") + language + ".wav");

	return narratorSoundUrl;
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

void fairytale::addClipPackage(ClipPackage *package)
{
	this->m_clipPackages.insert(package->id(), package);
}

void fairytale::playBonusClip(const fairytale::ClipKey &clipKey)
{
	if (m_playingBonusClip || this->isGameRunning())
	{
		return;
	}

	BonusClip *bonusClip = this->getBonusClipByKey(clipKey);

	if (bonusClip != nullptr)
	{
		this->m_playingBonusClip = true;
		this->m_player->playBonusVideo(this, bonusClip->videoUrl(), bonusClip->description());
	}
	else
	{
		qDebug() << "Missing Bonus Clip" << clipKey;
	}
}

void fairytale::changeLanguage()
{
	QAction *action = dynamic_cast<QAction*>(sender());

	qDebug() << "Change language to:" << action->text();

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

void fairytale::removeClipPackage(ClipPackage *package)
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

fairytale::Difficulty fairytale::defaultDifficulty() const
{
	return Difficulty::Normal;
}

bool fairytale::defaultUseMaxRounds() const
{
	return true;
}

int fairytale::defaultMaxRounds() const
{
	return 3;
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

fairytale::ClipPackages fairytale::defaultClipPackages() const
{
	ClipPackages result;

	for (ClipPackages::const_iterator iterator = this->clipPackages().begin(); iterator != this->clipPackages().end(); ++iterator)
	{
		if (iterator.key() == "gustav" || iterator.key() == "custom")
		{
			result.insert(iterator.key(), iterator.value());
		}
	}

	return result;
}

ClipPackage* fairytale::customClipPackage() const
{
	ClipPackages::const_iterator iterator = this->clipPackages().find("custom");

	if (iterator != this->clipPackages().end())
	{
		return iterator.value();
	}

	return nullptr;
}

bool fairytale::loadDefaultClipPackage()
{
	QList<QDir> dirs;
	dirs.push_back(this->defaultClipsDirectory());

	/*
	 * Copy the custom.xml file to the local home directory if it does not exist already.
	 * This is necessary since the file has to be writable and different per user.
	 */
	if (!ensureCustomClipsExistence())
	{
		qDebug() << "Custom Clips do not exist.";

		return false;
	}

	dirs.push_back(customClipsDirectory());

	QStringList defaultClipPackages;
	defaultClipPackages.push_back("gustav.xml");
	defaultClipPackages.push_back("custom.xml");

	int i = 0;

	foreach (const QString &defaultClipPackage, defaultClipPackages)
	{
		const QString filePath = dirs[i].filePath(defaultClipPackage);
		const QFileInfo fileInfo(filePath);

		qDebug() << "Loading default clip package from: " << filePath;

		if (fileInfo.exists() && fileInfo.isReadable())
		{
			ClipPackage *package = new ClipPackage(this);

			if (package->loadClipsFromFile(filePath))
			{
				this->addClipPackage(package);
			}
			else
			{
				delete package;
				package = nullptr;

				qDebug() << "Error on loading clip package:" << fileInfo.absoluteFilePath();
			}
		}
		else
		{
			qDebug() << "Default clip package does not exist:" << fileInfo.absoluteFilePath();
		}

		++i;
	}

	return !this->clipPackages().isEmpty();
}

bool fairytale::resetToDefaultClipPackages()
{
	this->m_clipPackages.clear();

#ifndef Q_OS_ANDROID
	// We need file:/ on other systems.
	const QUrl clipsDir = QUrl::fromLocalFile(this->defaultClipsDirectory());
#else
	// Dont prepend file:/ on Android!
	const QUrl clipsDir = QUrl(this->defaultClipsDirectory());
#endif
	setClipsDir(clipsDir);

	return loadDefaultClipPackage();
}

void fairytale::changePrimaryScreen(QScreen *screen)
{
	if (m_currentScreen != nullptr)
	{
		disconnect(m_currentScreen, &QScreen::availableGeometryChanged, this, &fairytale::changeAvailableGeometry);
	}

	m_currentScreen = screen;
	connect(m_currentScreen, &QScreen::availableGeometryChanged, this, &fairytale::changeAvailableGeometry);
}

void fairytale::changeAvailableGeometry(const QRect &geometry)
{
	qDebug() << "available geometry" << geometry;
	const QSize size = QSize(geometry.width(), geometry.height());
	this->setMaximumSize(size); // prevent widgets from expanding too wide!
	this->resize(QSize(geometry.width(), geometry.height()));
}

void fairytale::finishCentralDialog(int result)
{
	m_centralDialogResult = result;
	// Make sure the result cannot be changed anymore.
	disconnect(dynamic_cast<QDialog*>(QObject::sender()), &QDialog::finished, this, &fairytale::finishCentralDialog);
}

void fairytale::addCustomFairytale(CustomFairytale *customFairytale)
{
	CustomFairytale *alreadyExisting = nullptr;

	CustomFairytales::iterator iterator = m_customFairytales.find(customFairytale->name());

	if (iterator != m_customFairytales.end())
	{
		alreadyExisting = iterator.value();

		removeCustomFairytale(alreadyExisting);
	}

	this->m_customFairytales.insert(customFairytale->name(), customFairytale);
}

void fairytale::removeCustomFairytale(CustomFairytale *customFairytale)
{
	CustomFairytales::iterator iterator = this->m_customFairytales.find(customFairytale->name());

	if (iterator != this->m_customFairytales.end())
	{
		this->m_customFairytales.erase(iterator);
		delete customFairytale;
	}
}

BonusClip* fairytale::getBonusClipByKey(const ClipKey &key)
{
	ClipPackages::iterator iterator = m_clipPackages.find(key.first);

	if (iterator == m_clipPackages.end())
	{
		return nullptr;
	}

	ClipPackage *package = iterator.value();

	ClipPackage::BonusClips::const_iterator bonusClipIterator = package->bonusClips().find(key.second);

	if (bonusClipIterator == package->bonusClips().end())
	{
		return nullptr;
	}

	return bonusClipIterator.value();
}

BonusClipsDialog* fairytale::bonusClipsDialog()
{
	if (this->m_bonusClipsDialog == nullptr)
	{
		this->m_bonusClipsDialog = new BonusClipsDialog(this);
	}

	return this->m_bonusClipsDialog;
}

void fairytale::showBonusClipsDialog()
{
	this->execInCentralWidgetIfNecessary(bonusClipsDialog());
}

FairytalesDialog* fairytale::fairytalesDialog()
{
	if (this->m_fairytalesDialog == nullptr)
	{
		this->m_fairytalesDialog = new FairytalesDialog(this);
	}

	return this->m_fairytalesDialog;
}

void fairytale::showFairytalesDialog()
{
	this->execInCentralWidgetIfNecessary(fairytalesDialog());
}

LocaleDialog* fairytale::localeDialog()
{
	if (this->m_localeDialog == nullptr)
	{
		this->m_localeDialog = new LocaleDialog(this);
	}

	return this->m_localeDialog;
}

void fairytale::showLocaleDialog()
{
	this->execInCentralWidgetIfNecessary(localeDialog());
}

void fairytale::updatePixmap()
{
	const QFileInfo fileInfo(":/resources/splash" + this->currentTranslation() + ".jpg");

	if (fileInfo.exists())
	{
		logoLabel->setPixmap(QPixmap(fileInfo.absoluteFilePath()));
	}
	else
	{
		qDebug() << fileInfo.absoluteFilePath() << "does not exist!";
	}
}

void fairytale::setVideoSoundMuted(bool muted)
{
	this->m_player->setMuted(muted);
}

bool fairytale::isVideoSoundMuted() const
{
	return this->m_player->isMuted();
}

void fairytale::setVideoSoundVolume(int volume)
{
	this->m_player->setVolume(volume);
}

int fairytale::videoSoundVolume() const
{
	return this->m_player->volume();
}

#include "fairytale.moc"
