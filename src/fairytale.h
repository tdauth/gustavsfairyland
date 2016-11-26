#ifndef fairytale_H
#define fairytale_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QQueue>
#include <QtWidgets/QPushButton>
#include <QtMultimedia/QMediaPlayer>
#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QShortcut>
#include <QHash>

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

/**
 * \brief The fairytale application which provdes a main window and the basic logic of the game.
 */
class fairytale : public QMainWindow, protected Ui::MainWindow
{
	Q_OBJECT

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
		/**
		 * Enables or disables menu actions and buttons which are only enabled or disabled during playing a custom fairytale.
		 * This prevents the player from starting a new game before skipping the custom fairytale.
		 * \param enabled If this value is true the buttons and actions for a custom fairytale will be enabled and others will be disabled. Otherwise it is the other way around.
		 */
		void setCustomFairytaleButtonsEnabled(bool enabled);

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
		void settings();
		void openEditor();
		void showHighScores();
		void about();

		/**
		 * Starts the game with the first available package and game mode.
		 */
		void quickGame();

		/**
		 * Restarts the game with the same package and the same game mode.
		 */
		void retry();

		/**
		 * Updates the language of the application by applying a new translation file.
		 *
		 * \param language A locale string like "en" or "de".
		 * \return Returns true if the language file has been successfully loaded. Otherwise, for example if the file does not exist, it returns false.
		 */
		bool loadLanguage(const QString &language);

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

		/**
		 * Stores all game modes by their unique IDs.
		 */
		typedef QMap<QString, GameMode*> GameModes;
		/**
		 * Stores all clip packages by their unique IDs.
		 */
		typedef QMap<QString, ClipPackage*> ClipPackages;

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
		 * \return Returns the current screen's rect.
		 */
		static QRect screenRect();
		/**
		 * \return Returns the current screen's orientation.
		 */
		static Qt::ScreenOrientation screenOrientation();
		static QRect referenceRect();
		static Qt::ScreenOrientation referenceOrientation();
		/**
		 * Calculates a scaling factor depending on the current DPI settings and display size.
		 */
		static qreal screenWidthRatio();
		static qreal screenHeightRatio();
		/**
		 * Updates the size of \p widget and all sub widgets in its layout to the current screen size.
		 */
		static void updateSize(QWidget *widget);
		static QSize widgetSize(const QSize &currentSize);
		static qreal fontSize(int currentFontSize);
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
		 * Applies the custom style of the game to \p widget which means updating the background color
		 * and the font.
		 * \param widget The Widget of which the style is being updated.
		 */
		static void applyStyle(QWidget *widget);
		/**
		 * Applies the custom style to \p widget and all of its children widgets recursively.
		 */
		static void applyStyleRecursively(QWidget *widget);

		/**
		 * Starts a new game using all specified parameters.
		 */
		void startNewGame(ClipPackage *clipPackage, GameMode *gameMode, Difficulty difficulty, bool useMaxRounds, int maxRounds);

		/**
		 * Creates a new game of "Gustav's Fairyland". The game is represented by a main window.
		 */
		fairytale(Qt::WindowFlags flags = 0);
		virtual ~fairytale();

		/**
		 * \return Returns the directory where the clips are stored by default.
		 */
		QString defaultClipsDirectory() const;

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
		void win();
		void afterOutroWin();

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
		GameMode* defaultGameMode() const;
		Difficulty defaultDifficulty() const;
		bool defaultUseMaxRounds() const;
		int defaultMaxRounds() const;

		const ClipPackages& clipPackages() const;
		ClipPackage* defaultClipPackage() const;

		/**
		 * Resolves the URL of a clip using the configured clips directory if necessary.
		 * \param url The URL of a file for a clip. This might be an absolute or relative URL.
		 * \return Returns the resolved clip URL. If \p url is relative it prepends the directory \ref clipsDir(). Otherwise it returns the absolute URL.
		 */
		QUrl resolveClipUrl(const QUrl &url) const;

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
		 * \return Returns the current number of rounds.
		 */
		int rounds() const;

		SettingsDialog* settingsDialog();
		CustomFairytaleDialog* customFairytaleDialog();

		/**
		 * \return Returns true if the current turn of the current game mode requires a person clip. Otherwise, if it requires an act clip, it returns false.
		 */
		bool requiresPerson() const;
		/**
		 * \return Returns the currently used clip package.
		 */
		ClipPackage* clipPackage() const;
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
		typedef QList<Clip*> CompleteSolution;
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
		Clip* startPerson() const;
		QString description(Clip *startPersonClip, int turn, Clip *clip, bool markBold = true);

		/**
		 * Plays a sound if no sound is already played. Otherwise it doesn't play the sound at all.
		 * \param url The URL of the sound file.
		 * \return Returns true if the sound is played. Otherwise it returns false.
		 */
		bool playSound(const QUrl &url);

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

		/**
		 * Loads the default clip package and returns true if it exists and has successfully been loaded.
		 */
		bool loadDefaultClipPackage();

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
		 * \return Returns the result of the dialog's exec() call.
		 */
		int execInCentralWidgetIfNecessary(QDialog *dialog);

	protected:
		virtual void changeEvent(QEvent *event) override;
		virtual void showEvent(QShowEvent *event) override;

	private slots:
#ifdef Q_OS_ANDROID
		void finishNarratorAndroid();
#endif
		void finishNarrator(QMediaPlayer::State state);
		void onFinishVideoAndSounds();
		void finishAudio(QMediaPlayer::State state);
		void finishMusic(QMediaPlayer::State state);
		void timerTick();
		void playBonusClip();
		void changeLanguage();

		void playCustomFairytaleSlot();

		void changePrimaryScreen(QScreen *screen);
		void changeAvailableGeometry(const QRect &geometry);

		void finishCentralDialog(int result);

	private:
		/**
		 * Updates the text of the time label with the remaining time.
		 */
		void updateTimeLabel();
		void addCurrentSolution();
		void cleanupGame();
		void cleanupAfterOneGame();
		void hideGameWidgets();
		void finishPlayingCustomFairytale();
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
		Clip *m_startPerson;

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
		 * The currently selected clip package which is used to play.
		 */
		ClipPackage *m_clipPackage;

		/**
		 * State flag which indicates that the intro is currently played if its value is true.
		 */
		bool m_playIntro;
		/**
		 * State flag which indicates that the outro for winning the came is currently played if its value is true.
		 */
		bool m_playOutroWin;

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
		/// Flag which indicates if a new sound can be played at the moment.
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

		/**
		 * The key of every bonus clip is represented by the ID of its clip package and the ID of the clip itself.
		 * This key can be used to uniquely identify bonus clips even if several clip packages are being used.
		 */
		typedef QPair<QString, QString> BonusClipKey;

		BonusClip* getBonusClipByKey(const BonusClipKey &key);

		typedef QMap<BonusClipKey, bool> BonusClipUnlocks;
		BonusClipUnlocks m_bonusClipUnlocks;
		typedef QMap<QAction*, BonusClipKey> BonusClipActions;
		BonusClipActions m_bonusClipActions;
		bool m_playingBonusClip;

		QTranslator m_translator;
		typedef QMap<QAction*, QString> TranslationFileNames;
		TranslationFileNames m_translationFileNames;
		QString m_currentTranslation;

		CustomFairytales m_customFairytales;
		typedef QMap<QAction*, CustomFairytale*> CustomFairytaleActions;
		CustomFairytaleActions m_customFairytaleActions;

		CustomFairytale *m_playingCustomFairytale;
		int m_customFairytaleIndex;

		QShortcut *m_pauseGameShortcut;
		QShortcut *m_cancelGameShortcut;

		QScreen *m_currentScreen;

		int m_centralDialogResult;

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
	foreach (QAction *action, this->m_bonusClipActions.keys())
	{
		delete action;
	}

	this->m_bonusClipActions.clear();
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

inline int fairytale::rounds() const
{
	return (this->m_turns - 1) / 2;
}

inline bool fairytale::requiresPerson() const
{
	return this->m_requiresPerson;
}

inline ClipPackage* fairytale::clipPackage() const
{
	return this->m_clipPackage;
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

inline Clip* fairytale::startPerson() const
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

inline const fairytale::CustomFairytales& fairytale::customFairytales() const
{
	return this->m_customFairytales;
}

#endif // fairytale_H
