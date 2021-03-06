#ifndef fairytale_H
#define fairytale_H

#include <functional>

#include <QMainWindow>
#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QList>
#include <QQueue>
#include <QPushButton>
#include <QMediaPlayer>
#include <QTranslator>
#include <QDir>
#include <QShortcut>
#include <QHash>
#include <QAudioOutputSelectorControl>
#include <QAudioInputSelectorControl>

#include "ui_mainwindow.h"

class Clip;
class BonusClip;
class Player;
class SettingsDialog;
class ClipsDialog;
class ClipPackageDialog;
class GameModeDialog;
class ClipPackage;
class ClipPackageEditor;
class CustomFairytaleDialog;
class GameMode;
class AboutDialog;
class WonDialog;
class GameOverDialog;
class HighScores;
class CustomFairytale;
class BonusClipsDialog;
class FairytalesDialog;
class LocaleDialog;

/**
 * \brief The fairytale application which provdes a main window and the basic logic of the game.
 */
class fairytale : public QMainWindow, protected Ui::MainWindow
{
	Q_OBJECT

	public:
		/**
		 * Depending on the game mode the difficulty has influence on the gameplay and makes it harder or easier.
		 */
		enum class Difficulty
		{
			Easy, /// With this difficulty should be even retarded people (no offense) should be able to finish the game.
			Normal, /// With a limited number of rounds it should be possible to finish with a decent time but with more rounds it could be harder to finish successfully.
			Hard, /// Only professionals can finish the game successfully with this difficulty.
			Mahlerisch /// Impossible difficulty since Mahler himself was a genius.
		};

	public slots:
		/**
		 * Starts a new game.
		 * First shows a dialog to select the clip package which should be played.
		 * After that the game begins.
		 */
		void newGame();
		/**
		 * Changes to the next turn.
		 * In every turn a narrator video is played which can be skipped and then the player has to select a clip in a specified time.
		 */
		void nextTurn();

		/**
		 * Clears all clip buttons from the solution widget.
		 */
		void clearSolution();

		/**
		 * Enables or disables menu actions and buttons which are only enabled during a game.
		 * \param enabled If this value is true the buttons and actions will be enabled. Otherwise they will be disabled.
		 */
		void setGameButtonsEnabled(bool enabled);

		void playFinalVideo();

		/**
		 * Pauses the game if it is running. Otherwise if it is paused the game will be resumed.
		 */
		void pauseGameAction();
		/**
		 * Cancels the current game. Has no effect if no game is running.
		 */
		void cancelGame();
		void showCustomFairytale();
		/**
		 * Shows the settings dialog.
		 */
		void showSettings();
		void openEditor();
		/**
		 * Shows the high scores dialog.
		 */
		void showHighScores();
		/**
		 * Allows creating one custom clip which can be used in the game.
		 */
		void record();
		/**
		 * Shows the about dialog which contains the credits.
		 */
		void about();

		/**
		 * Starts the game with the first available package and game mode.
		 */
		void quickGame();

		/**
		 * Restarts the game with the same package and the specified settings.
		 */
		void retry(GameMode *gameMode, Difficulty difficulty);

		void showLocaleDialog();
		/**
		 * Updates the language of the application by applying a new translation file.
		 *
		 * \param language A locale string like "en" or "de".
		 * \return Returns true if the language file has been successfully loaded. Otherwise, for example if the file does not exist, it returns false.
		 */
		bool loadLanguage(const QString &language);

	public:
		/**
		 * Stores all game modes by their unique IDs.
		 */
		typedef QMap<QString, GameMode*> GameModes;
		/**
		 * Stores all clip packages by their unique IDs.
		 */
		typedef QMap<QString, ClipPackage*> ClipPackages;

		/**
		 * The key of every bonus clip is represented by the ID of its clip package and the ID of the clip itself.
		 * This key can be used to uniquely identify bonus clips even if several clip packages are being used.
		 */
		class ClipKey : public QPair<QString, QString>
		{
			public:
				ClipKey()
				{
				}

				ClipKey(const QString &packageId, const QString &clipId) : QPair<QString, QString>(packageId, clipId)
				{
				}

				bool isEmpty() const
				{
					return this->first.isEmpty() && this->second.isEmpty();
				}
		};

		/**
		 * The default music volume in percentage from 0 to 100.
		 */
		static const int defaultMusicVolume = 25;
		static const int defaultClickSoundsVolume = 100;
		static const int defaultVideoSoundVolume = 100;

		/**
		 * Default game style which is applied in the beginning to qApp.
		 *
		 * \{
		 */
		static QPalette gameColorPalette();
		static QString gameStyleSheet();
		static QFont gameFont();
		/**
		 * \}
		 */

		/**
		 * \return Returns true if the user has a touch device to press at the floating clips.
		 */
		static bool hasTouchDevice();

		/**
		 * Screen utility functions to scale widgets to the current screen where the game is played.
		 *
		 * \{
		 */
		/**
		 * \return Returns the current screen's rect of available geometry.
		 */
		QRect screenRect();
		/**
		 * \return Returns the available geometry rect which was used to design the user interface.
		 */
		QRect referenceRect();
		/**
		 * Calculates a scaling factor depending on the current display size.
		 */
		qreal screenWidthRatio();
		qreal screenHeightRatio();
		/**
		 * The pixel ratio uses (height + width) / 2.
		 * It is used for the font size since the font uses width AND height.
		 */
		qreal pixelRatio();
		/**
		 * Updates the size of \p widget and all sub widgets in its layout to the current screen size.
		 * It uses \ref referenceRect() to determine the ratio between the current available geometry on the current screen and the used available geometry
		 * to design the user interface.
		 *
		 * It stores the original designed size of all widgets for a later reference when the available geometry changes.
		 *
		 * \param widget The widget of which the size is updated. The size is also updated of all child widgets.
		 */
		void updateSize(QWidget *widget);
		/**
		 * \return Returns the new wiget size which is calculated by the ratio of the \ref referenceRect() and available geometry on the screen.
		 */
		QSize widgetSize(const QSize &currentSize);
		qreal fontSize(int currentFontSize);
		/**
		 * \}
		 */

		/**
		 * Converts a locale string to a human readable string.
		 * Locale strings may be "en" or "de" for example which would be converted to "English" and "German".
		 * \param locale A short string which represents language.
		 * \return Returns a human readable string representation of the locale.
		 */
		static QString localeToName(const QString &locale);

		/**
		 * Starts a new game using all specified parameters.
		 */
		void startNewGame(const ClipPackages &clipPackages, GameMode *gameMode, Difficulty difficulty, bool useMaxRounds, int maxRounds);

		/**
		 * Creates a new game of "Gustav's Fairyland". The game is represented by a main window.
		 */
		explicit fairytale(Qt::WindowFlags flags = 0);
		virtual ~fairytale();

		/**
		 * \return Returns all supported languages by checking for translation files in \ref translationsDir(). The languages are returned as locale names like "en" or "de".
		 */
		QStringList languages() const;

		/**
		 * \return Returns the directory where the clips are stored by default.
		 */
		QString defaultClipsDirectory() const;
		/**
		 * Ensures that the user has a file in QStandardPaths::AppDataLocation/.gustavsfairyland called "custom.xml" which is used for all
		 * custom clips.
		 * \return Returns true if the file exists or has been created by this function. Otherwise it returns false.
		 */
		bool ensureCustomClipsExistence();
		QString customClipsParentDirectory() const;
		QString customClipsDirectory() const;

		/**
		 * Event handler which is called by the current game mode.
		 */
		void onFinishTurn();

		/**
		 * Plays a specific clip of the complete solution with \p index.
		 * If the index is invalid, nothing happens.
		 */
		void playFinalClip(int index);

		void gameOver();
		void afterOutroGameOver();
		void win();
		void afterOutroWin();
		/**
		 * Unlocks a random bonus clip if not all bonus clips have already been unlocked. Otherwise it shows a message box with this information.
		 * When a bonus clip is unlocked it shows the user a message box asking him if he/she wants to see the bonus clip now.
		 * If the user clicks "Yes" the bonus clip is being played immediately.
		 * \return Returns true if the bonus clip is being played immediately. Otherwise it returns false.
		 */
		bool unlockRandomBonusClip();
		/**
		 * Adds a new high scores entry and shows the custom fairytale dialog.
		 * This has to be called after unlocking a bonus clip. Either after it has been played or directly afterwards.
		 */
		void afterUnlockingBonusClip();

		bool askForPlayingFairytale();
		void afterAskingForPlayingFairytale();

		bool isMediaPlayerPaused() const;
		bool isMediaPlayerPlaying() const;

		bool isGamePaused() const;
		/// \return Returns true if the game is running. Returns also true if the game is paused, yet it is still running in this case. Returns false if no game is running.
		bool isGameRunning() const;
		void pauseGame();
		void resumeGame();

		bool isTimerPaused() const;
		bool isTimerRunning() const;
		void pauseTimer();
		void resumeTimer();

		void addClipPackage(ClipPackage *package);
		void setClipPackages(const ClipPackages &packages);
		void removeClipPackage(ClipPackage *package);
		const GameModes& gameModes() const;
		/**
		 * The default game mode is "sequence".
		 *
		 * \return Returns the default game mode which is used when a game is started via the "Let's Go" button.
		 */
		GameMode* defaultGameMode() const;
		Difficulty defaultDifficulty() const;
		bool defaultUseMaxRounds() const;
		int defaultMaxRounds() const;

		/**
		 * \return Returns all clip packages of the game which are loaded at the moment.
		 */
		const ClipPackages& clipPackages() const;
		/**
		 * \return Returns the clip package which is used by default. This is usually "gustav". If it does not exist, it uses the first one. Returns nullptr if no clip packages have been loaded.
		 */
		ClipPackage* defaultClipPackage() const;
		/**
		 * \return Returns the clip packages which are used by default (for example when starting a quick game). These include "gustav" and "custom".
		 */
		ClipPackages defaultClipPackages() const;
		/**
		 * \return Returns the custom clip package "custom" which is used for all customly added/recorded clips. Returns nullptr if the clip package has not been loaded.
		 */
		ClipPackage* customClipPackage() const;

		/**
		 * Resolves the URL of a clip using the configured clips directory if necessary.
		 * \param url The URL of a file for a clip. This might be an absolute or relative URL.
		 * \return Returns the resolved clip URL. If \p url is relative it prepends the directory \ref clipsDir(). Otherwise it returns the absolute URL.
		 */
		QUrl resolveClipUrl(const QUrl &url) const;

		/**
		 * Converts \p url to a local file path.
		 * This is different on Android since "assets:/" is used. Therefore on Android it uses url() of the URL to get the file path as a string.
		 * On the other platforms it uses toLocalFile() to get the file path.
		 * Therefore the method works cross-platform.
		 * \param url The URL which is converted to a string.
		 * \return Returns the string representation of the URL.
		 */
		static QString filePath(const QUrl &url);

		/**
		 * The directory where all clips are stored.
		 *
		 * This directory is used in \ref resolveClipUrl() to prepend an absolute directory path to relative clip URLs.
		 * \{
		 */
		void setClipsDir(const QUrl &url);
		QUrl clipsDir() const;
		/**
		 * \}
		 */

		/**
		 * \return Returns the current number of turns.
		 */
		int turns() const;
		/**
		 * \return Returns the current number of rounds. If the game mode doesn't use a maximum number of rounds, it does always return 1.
		 */
		int rounds() const;


		LocaleDialog* localeDialog();

		SettingsDialog* settingsDialog();
		CustomFairytaleDialog* customFairytaleDialog();

		/**
		 * \return Returns true if the current turn of the current game mode requires a person clip. Otherwise, if it requires an act clip, it returns false.
		 */
		bool requiresPerson() const;
		/**
		 * \return Returns the currently used clip package.
		 */
		const ClipPackages& currentClipPackages() const;
		/**
		 * \return Returns the currently used game mode.
		 */
		GameMode* gameMode() const;
		/**
		 * \return Returns the currently specified difficulty.
		 */
		Difficulty difficulty() const;
		/**
		 * \return Returns true if the current game has a limited number of maximum rounds. Otherwise the maximum is defined by the number of clips the currently selected clip package offers. In this case it returns false.
		 */
		bool useMaxRounds() const;
		/**
		 * \return Returns the currently specified maximum number of rounds for the current game. This value has only an effect if \ref useMaxRounds(). Otherwise it is ignored.
		 */
		int maxRounds() const;


		/**
		 * \brief A list of clips for the solution of a complete game.
		 *
		 * The clips create a custom fairytale in the order they have been appended to the list.
		 * The first clip is always the starting person.
		 */
		typedef QList<ClipKey> CompleteSolution;
		/**
		 * \return Returns the current complete solution. It depends on how many turns the player has played successfully.
		 */
		const CompleteSolution& completeSolution() const;

		QWidget* gameAreaWidget() const;
		QGridLayout* gameAreaLayout() const;

		AboutDialog* aboutDialog();
		WonDialog* wonDialog();
		GameOverDialog* gameOverDialog();
		HighScores* highScores() const;

		/**
		 * \note Is only set during a game.
		 */
		ClipKey startPerson() const;
		QString description(Clip *startPersonClip, int turn, Clip *clip, bool markBold = true);

		/**
		 * Plays a sound if no sound is already played. Otherwise it doesn't play the sound at all.
		 * \param url The URL of the sound file.
		 * \param immediately If this value is true, the currently played sound will be ignored and the sound from \p url will be played immediately. Otherwise the sound from \p url won't be played if there is already a sound being played at the moment.
		 * \return Returns true if the sound is played. Otherwise it returns false.
		 */
		bool playSound(const QUrl &url, bool immediately = false);
		bool isSoundPlaying() const;

		/**
		 * Sound data to queue for the \ref Player to play a sound.
		 */
		struct PlayerSoundData
		{
			QUrl narratorSoundUrl;
			QUrl imageUrl;
			QString description;
			bool prefix;
		};

		/**
		 * Plays the sound immediately if the queue is empty. Otherwise it queues the sound.
		 */
		void queuePlayerSound(const PlayerSoundData &soundData);

		static QString systemLocale();
		/**
		 * \return Returns the directory which stores all translation files of this application.
		 */
		QDir translationsDir() const;
		/**
		 * \return Returns the locale string of the current translation.
		 */
		QString currentTranslation() const;

		/**
		 * Starts some random music composed by the greatest composer of all time: Gustav Mahler.
		 * The player should win every game in seconds since this music gives him the greatest joy of all time.
		 */
		void startMusic();
		void setMusicMuted(bool muted);
		bool isMusicMuted() const;
		void setMusicVolume(int volume);
		int musicVolume() const;

		void setAudioPlayerMuted(bool muted);
		bool isAudioPlayerMuted() const;
		void setAudioPlayerVolume(int volume);
		int audioPlayerVolume() const;

		void setNarratorSoundMuted(bool muted);
		bool isNarratorSoundMuted() const;
		void setNarratorSoundVolume(int volume);
		int narratorSoundVolume() const;

		void setVideoSoundMuted(bool muted);
		bool isVideoSoundMuted() const;
		void setVideoSoundVolume(int volume);
		int videoSoundVolume() const;


		/**
		 * Loads the default clip package and returns true if it exists and has successfully been loaded.
		 */
		bool loadDefaultClipPackage();
		/**
		 * Resets the clips directory to the default path and tries to load the default clip packages.
		 */
		bool resetToDefaultClipPackages();

		/**
		 * \note If the name is already used the current custom fairytale will be overwritten. The name must be a unique key.
		 */
		void addCustomFairytale(CustomFairytale *customFairytale);
		void removeCustomFairytale(CustomFairytale *customFairytale);

		void playCustomFairytaleClip(int index);
		void playCustomFairytale(CustomFairytale *customFairytale);

		typedef QMap<QString, CustomFairytale*> CustomFairytales;
		const CustomFairytales& customFairytales() const;

		typedef QList<QWidget*> Widgets;
		/**
		 * Hides all widgets in the central widget of the main window (except the central widget itself) and returns a list of them.
		 * \return Returns all widgets which are hidden by this method.
		 */
		Widgets hideWidgetsInMainWindow();
		/**
		 * Shows all widgets from \p widgets.
		 * \param widgets A list of widgets which is shown again.
		 */
		void showWidgetsInMainWindow(Widgets widgets);
		/**
		 * On the Android platform modal dialogs do not work. All widgets should be shown in the top level main window.
		 * This function executes a dialog as widget part of the central widget of the main window if the application is run on Android.
		 * Otherwise the dialog is executed normally.
		 *
		 * \param dialog The dialog for which exec() is called but which is placed in the central widget if necessary.
		 * \param lambda This function is called after the dialog is shown but before the event loop and waiting for execution starts.
		 * \return Returns the result of the dialog's exec() call.
		 */
		int execInCentralWidgetIfNecessaryEx(QDialog *dialog, std::function<void(QDialog*)> &&lambda);
		int execInCentralWidgetIfNecessary(QDialog *dialog);

		void showSettingsEx(std::function<void(QDialog*)> &&lambda);

		/**
		 * Gets a clip package by its unique ID.
		 * \param packageId The unique package ID.
		 * \return Returns the clip package if it is loaded. Otherwise it returns nullptr.
		 */
		ClipPackage* getClipPackageById(const QString &packageId);
		/**
		 * Gets a clip by its unique package and its unique clip ID.
		 * \param clipKey The unique clip key.
		 * \return Returns the clip if the package is loaded and the clip is found. Otherwise it returns nullptr.
		 */
		Clip* getClipByKey(const ClipKey &clipKey);

		/**
		 * \return Returns the maximum number of possible rounds if all clips from all \p clipPackages would be used.
		 *
		 * The maximum number of possible rounds depends on the number of persons and acts. One person is used in the beginning and has to be substracted.
		 * Then the minimum number of the number of persons and the number of acts is the maximum of rounds.
		 */
		int maxRoundsByMultipleClipPackages(const ClipPackages &clipPackages);

		QAudioOutputSelectorControl* audioOutputSelectorControl() const;
		QAudioInputSelectorControl* audioInputSelectorControl() const;

		BonusClip* getBonusClipByKey(const ClipKey &key);
		/**
		 * \brief BonusClipUnlocks stores all entries for Bonus Clips which are unlocked.
		 *
		 * Every bonus clip is identified by its unique key.
		 */
		typedef QSet<ClipKey> BonusClipUnlocks;
		const BonusClipUnlocks& bonusClipUnlocks() const;

		BonusClipsDialog* bonusClipsDialog();
		void showBonusClipsDialog();
		void playBonusClip(const fairytale::ClipKey &clipKey, bool duringGame = false);

		FairytalesDialog* fairytalesDialog();
		void showFairytalesDialog();

		void addCurrentSolution();

	protected:
		virtual void changeEvent(QEvent *event) override;
		virtual void showEvent(QShowEvent *event) override;

	private slots:
		void onVideoAndSoundStateChanged(QMediaPlayer::State state);
		void onFinishVideoAndSounds();
		void finishAudio(QMediaPlayer::State state);
		void finishMusic(QMediaPlayer::State state);
		void timerTick();
		void changeLanguage();

		void changePrimaryScreen(QScreen *screen);
		void changeAvailableGeometry(const QRect &geometry);

		void finishCentralDialog(int result);

		void unlockAllBonusClips();
		void lockAllBonusClips();

	private:
		/**
		 * Game buttons use icons only on Android. This method clears the button texts on Android.
		 */
		void setupGameButtons();

		/**
		 * Updates the logo depending on the current language of the application.
		 * The logo contains the name of the application in the current language.
		 */
		void updatePixmap();
		/**
		 * Updates the text of the time label with the remaining time.
		 */
		void updateTimeLabel();
		void cleanupGame();
		void cleanupAfterOneGame();
		void hideGameWidgets();
		void finishPlayingCustomFairytale();

		/**
		 * Updates the timer and description laben and starts the timer if necessary. Calls GameMode::afterNarrator().
		 */
		void afterNarrator();
		/**
		  * \return Returns the URL to the "and" sound in the current language.
		  */
		QUrl narratorSoundUrl() const;

		/// The directory which is used to resolve relative paths of files for clips from clip packages.
		QUrl m_clipsDir;

		/**
		 * The currently played turns.
		 */
		int m_turns;

		/**
		 * The initial clip for the initial person who is the protagonist of the fairytale.
		 */
		ClipKey m_startPerson;

		/**
		 * The video player for clips and narrator clips.
		 */
		Player *m_player;

		SettingsDialog *m_settingsDialog;
		ClipsDialog *m_clipsDialog;
		ClipPackageDialog *m_clipPackageDialog;

		/**
		 * The editor for creating custom clip packages.
		 */
		ClipPackageEditor *m_editor;

		/**
		 * The dialog to play the final custom fairytale of the game.
		 */
		CustomFairytaleDialog *m_customFairytaleDialog;

		/**
		 * The timer which is used for the limited time the player has to select the proper clip.
		 */
		QTimer m_timer;
		/**
		 * The remaining time of the timer in MS.
		 */
		long int m_remainingTime;
		/// The total elapsed time for one game.
		long int m_totalElapsedTime;

		/**
		 * If this value is true the current turn requires a person clip. Otherwise it requires an act clip.
		 */
		bool m_requiresPerson;

		/**
		 * All available game modes.
		 */
		GameModes m_gameModes;
		/**
		 * All loaded clip packages of the game.
		 */
		ClipPackages m_clipPackages;
		/**
		 * The currently selected clip packages which are used to play.
		 */
		ClipPackages m_currentClipPackages;

		/**
		 * State flag which indicates that the intro is currently played if its value is true.
		 */
		bool m_playIntro;
		/**
		 * State flag which indicates that the outro for winning the game is currently played if its value is true.
		 */
		bool m_playOutroWin;
		/**
		 * State flag which indicates that the outro for losing the game is currently played if its value is true.
		 */
		bool m_playOutroLose;

		CompleteSolution m_completeSolution;
		int m_completeSolutionIndex;
		bool m_playCompleteSolution;

		bool m_paused;
		bool m_pausedTimer;
		bool m_isRunningTimer;
		bool m_pausedMediaPlayer;
		bool m_isPlayingMediaPlayer;
		bool m_isRunning;

		/// Custom audio player which allows only to play one sound at once and discards all other sounds which are played during that time.
		QMediaPlayer *m_audioPlayer;
		/// Flag which indicates if a new sound can be played at the moment. This flag becomes false when the audio player state changes to playing and becomes true when the state becomes stopped.
		bool m_playNewSound;
		/// A queue with sounds waiting for the player to become available.
		QQueue<PlayerSoundData> m_playerSounds;
		/// Player for background music.
		QMediaPlayer *m_musicPlayer;

		/**
		 * The currently played game mode.
		 */
		GameMode *m_gameMode;
		Difficulty m_difficulty;
		bool m_useMaxRounds;
		int m_maxRounds;

		/// This dialog appears when the "About" action is triggered.
		AboutDialog *m_aboutDialog;
		/// This dialog appears when a game is won.
		WonDialog *m_wonDialog;
		GameOverDialog *m_gameOverDialog;

		HighScores *m_highScores;

		BonusClipUnlocks m_bonusClipUnlocks;

		bool m_playingBonusClip;
		bool m_playingBonusClipDuringGame;

		QTranslator m_qtTranslator;
		QTranslator m_qtBaseTranslator;
		QTranslator m_translator;
		typedef QMap<QAction*, QString> TranslationFileNames;
		TranslationFileNames m_translationFileNames;
		QString m_currentTranslation;

		CustomFairytales m_customFairytales;

		CustomFairytale *m_playingCustomFairytale;
		int m_customFairytaleIndex;

		QShortcut *m_pauseGameShortcut;
		QShortcut *m_cancelGameShortcut;

		QScreen *m_currentScreen;

		int m_centralDialogResult;

		BonusClipsDialog *m_bonusClipsDialog = nullptr;

		FairytalesDialog *m_fairytalesDialog = nullptr;

		LocaleDialog *m_localeDialog = nullptr;

		/**
		 * \brief Size data for a widget which can be stored.
		 */
		struct WidgetSizeData
		{
			QSize size;
			QFont font;
		};

		/**
		 * \brief Allows fast access by a widget to its initial size data.
		 */
		typedef QHash<QWidget*, WidgetSizeData> WidgetSizes;
		/**
		 * Stores the initial size data for widgets.
		 * This data is used as a reference when scaling them in \ref updateSize().
		 */
		static WidgetSizes m_widgetSizes;

		/**
		 * All widgets which have currently been executed by \ref execInCentralWidgetIfNecessaryEx().
		 */
		QList<QWidget*> m_centralWidgets;
};

inline bool fairytale::isMediaPlayerPaused() const
{
	return this->m_pausedMediaPlayer;
}

inline bool fairytale::isMediaPlayerPlaying() const
{
	return this->m_isPlayingMediaPlayer;
}

inline void fairytale::setClipPackages(const fairytale::ClipPackages& packages)
{
	this->m_clipPackages.clear();

	for (ClipPackages::const_iterator iterator = packages.constBegin(); iterator != packages.constEnd(); ++iterator)
	{
		addClipPackage(iterator.value());
	}
}

inline const fairytale::GameModes& fairytale::gameModes() const
{
	return this->m_gameModes;
}

inline const fairytale::ClipPackages& fairytale::clipPackages() const
{
	return this->m_clipPackages;
}

inline void fairytale::setClipsDir(const QUrl &url)
{
	this->m_clipsDir = url;
}

inline QUrl fairytale::clipsDir() const
{
	return this->m_clipsDir;
}

inline int fairytale::turns() const
{
	return this->m_turns;
}

inline bool fairytale::requiresPerson() const
{
	return this->m_requiresPerson;
}

inline const fairytale::ClipPackages& fairytale::currentClipPackages() const
{
	return this->m_currentClipPackages;
}

inline bool fairytale::isGameRunning() const
{
	return this->m_isRunning;
}

inline GameMode* fairytale::gameMode() const
{
	return this->m_gameMode;
}

inline fairytale::Difficulty fairytale::difficulty() const
{
	return this->m_difficulty;
}

inline bool fairytale::useMaxRounds() const
{
	return this->m_useMaxRounds;
}

inline int fairytale::maxRounds() const
{
	return this->m_maxRounds;
}

inline const fairytale::CompleteSolution& fairytale::completeSolution() const
{
	return this->m_completeSolution;
}

inline QWidget* fairytale::gameAreaWidget() const
{
	return Ui::MainWindow::gameAreaWidget;
}

inline QGridLayout* fairytale::gameAreaLayout() const
{
	return Ui::MainWindow::gameAreaLayout;
}

inline HighScores* fairytale::highScores() const
{
	return this->m_highScores;
}

inline fairytale::ClipKey fairytale::startPerson() const
{
	return this->m_startPerson;
}

inline QString fairytale::currentTranslation() const
{
	return this->m_currentTranslation;
}

inline void fairytale::setMusicMuted(bool muted)
{
	this->m_musicPlayer->setMuted(muted);
}

inline bool fairytale::isMusicMuted() const
{
	return this->m_musicPlayer->isMuted();
}

inline void fairytale::setMusicVolume(int volume)
{
	this->m_musicPlayer->setVolume(volume);
}

inline int fairytale::musicVolume() const
{
	return this->m_musicPlayer->volume();
}

inline void fairytale::setAudioPlayerMuted(bool muted)
{
	this->m_audioPlayer->setMuted(muted);
}

inline bool fairytale::isAudioPlayerMuted() const
{
	return this->m_audioPlayer->isMuted();
}

inline void fairytale::setAudioPlayerVolume(int volume)
{
	this->m_audioPlayer->setVolume(volume);
}

inline int fairytale::audioPlayerVolume() const
{
	return this->m_audioPlayer->volume();
}

inline const fairytale::CustomFairytales& fairytale::customFairytales() const
{
	return this->m_customFairytales;
}

inline const fairytale::BonusClipUnlocks& fairytale::bonusClipUnlocks() const
{
	return this->m_bonusClipUnlocks;
}

#endif // fairytale_H