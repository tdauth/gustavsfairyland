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

#include "ui_mainwindow.h"

class Clip;
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
class HighScores;

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

		void playFinalVideo();

		/**
		 * Pauses the game if it is running. Otherwise if it is paused the game will be resumed.
		 */
		void pauseGame();
		void cancelGame();
		void showCustomFairytale();
		void settings();
		void openClipsDialog();
		void openEditor();
		ClipPackage* selectClipPackage();
		GameMode* selectGameMode();
		void showHighScores();
		void about();

	public:
		typedef QList<GameMode*> GameModes;
		typedef QList<ClipPackage*> ClipPackages;

		fairytale(Qt::WindowFlags flags = 0);
		virtual ~fairytale();

		/**
		 * Event handler which is called by the current game mode.
		 */
		void onFinishTurn();

		void playFinalClip(int index);

		void gameOver();
		void win();

		void addClipPackage(ClipPackage *package);
		void setClipPackages(const ClipPackages &packages);
		void removeClipPackage(ClipPackage *package);
		const GameModes& gameModes() const;
		const ClipPackages& clipPackages() const;

		QUrl resolveClipUrl(const QUrl &url) const;

		void setClipsDir(const QUrl &url);
		QUrl clipsDir() const;

		int turns() const;

		CustomFairytaleDialog* customFairytaleDialog();

		bool requiresPerson() const;
		ClipPackage* clipPackage() const;
		bool isPaused() const;
		bool isRunning() const;
		GameMode* gameMode() const;

		typedef QList<Clip*> CompleteSolution;
		const CompleteSolution& completeSolution() const;

		QGridLayout* gameAreaLayout() const;

		AboutDialog* aboutDialog();
		WonDialog* wonDialog();
		HighScores* highScores() const;

		QString description(int turn, Clip *clip, bool markBold = true);

		/**
		 * Plays a sound if no sound is already played. Otherwise it doesn't play the sound at all.
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

	private slots:
		void finishNarrator(QMediaPlayer::State state);
		void finishAudio(QMediaPlayer::State state);
		void timerTick();

	private:
		void updateTimeLabel();
		void addCurrentSolution();
		void cleanupGame();
		void cleanupAfterOneGame();


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
		GameModeDialog *m_gameModeDialog;

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

		CompleteSolution m_completeSolution;
		int m_completeSolutionIndex;
		bool m_playCompleteSolution;

		bool m_paused;
		bool m_pausedTimer;
		bool m_isRunning;

		/// Custom audio player which allows only to play one sound at once and discards all other sounds which are played during that time.
		QMediaPlayer *m_audioPlayer;
		/// Flag which indicates if a new sound can be played at the moment.
		bool m_playNewSound;
		/// A queue with sounds waiting for the player to become available.
		QQueue<PlayerSoundData> m_playerSounds;

		/**
		 * The currently played game mode.
		 */
		GameMode *m_gameMode;

		/// This dialog appears when the "About" action is triggered.
		AboutDialog *m_aboutDialog;
		/// This dialog appears when a game is won.
		WonDialog *m_wonDialog;

		HighScores *m_highScores;
};

inline void fairytale::addClipPackage(ClipPackage* package)
{
	this->m_clipPackages.push_back(package);
}

inline void fairytale::setClipPackages(const fairytale::ClipPackages& packages)
{
	this->m_clipPackages = packages;
}

inline void fairytale::removeClipPackage(ClipPackage* package)
{
    this->m_clipPackages.removeAll(package);
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

inline ClipPackage* fairytale::clipPackage() const
{
	return this->m_clipPackage;
}

inline bool fairytale::isPaused() const
{
	return this->m_paused;
}

inline bool fairytale::isRunning() const
{
	return this->m_isRunning;
}

inline GameMode* fairytale::gameMode() const
{
	return this->m_gameMode;
}

inline const fairytale::CompleteSolution& fairytale::completeSolution() const
{
	return this->m_completeSolution;
}

inline QGridLayout* fairytale::gameAreaLayout() const
{
	return centralLayout;
}

inline HighScores* fairytale::highScores() const
{
	return this->m_highScores;
}

#endif // fairytale_H
