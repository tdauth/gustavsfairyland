#ifndef fairytale_H
#define fairytale_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QList>

#include <QtMultimedia/QMediaPlayer>

#include "ui_mainwindow.h"

class Clip;
class Player;
class ClipsDialog;
class ClipPackageDialog;
class ClipPackage;
class ClipPackageEditor;
class CustomFairytaleDialog;

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
		 * Clears all clip buttons.
		 */
		void clearClipButtons();
		void clearSolution();
		void clearClips();

		/**
		 * Clears clip buttons, solution buttons and clips as well as stored solution clips etc.
		 */
		void clearAll();

		/**
		 * Enables or disables menu actions and buttons which are only enabled during a game.
		 * \param enabled If this value is true the buttons and actions will be enabled. Otherwise they will be disabled.
		 */
		void setGameButtonsEnabled(bool enabled);

		void playFinalVideo();

		void pauseGame();
		void showCustomFairytale();
		void openClipsDialog();
		void openEditor();
		ClipPackage* selectClipPackage();
		void about();

	public:
		typedef QList<ClipPackage*> ClipPackages;

		fairytale();
		virtual ~fairytale();

		void playFinalClip(int index);

		void gameOver();

		void addClipPackage(ClipPackage *package);
		void setClipPackages(const ClipPackages &packages);
		const ClipPackages& clipPackages() const;

		QUrl resolveClipUrl(const QUrl &url) const;

		QUrl clipsDir() const;

		CustomFairytaleDialog* customFairytaleDialog();

		bool isPaused() const;
		bool isRunning() const;

	private slots:
		void finishNarrator(QMediaPlayer::State state);
		void timerTick();

		void clickCard();

	private:
		void updateTimeLabel();
		void addCurrentSolution();
		void fillCurrentClips();
		void selectRandomSolution();
		QString description(int turn);


		QUrl m_clipsDir;

		int m_turns;

		Clip *m_startPerson;

		Player *m_player;

		ClipsDialog *m_clipsDialog;
		ClipPackageDialog *m_clipPackageDialog;

		ClipPackageEditor *m_editor;

		/**
		 * The dialog to play the final custom fairytale of the game.
		 */
		CustomFairytaleDialog *m_customFairytaleDialog;

		QTimer m_timer;
		long int m_remainingTime;

		bool m_requiresPerson;

		ClipPackage *m_clipPackage;
		QList<Clip*> m_clips;

		Clip *m_currentSolution;
		QVector<QPushButton*> m_buttons;
		QVector<Clip*> m_currentClips;
		ClipPackages m_clipPackages;

		QList<Clip*> m_completeSolution;
		QList<QPushButton*> m_completeSolutionButtons;
		int m_completeSolutionIndex;
		bool m_playCompleteSolution;

		bool m_paused;
		bool m_isRunning;
};

inline void fairytale::addClipPackage(ClipPackage* package)
{
	this->m_clipPackages.push_back(package);
}

inline void fairytale::setClipPackages(const fairytale::ClipPackages& packages)
{
	this->m_clipPackages = packages;
}

inline const fairytale::ClipPackages& fairytale::clipPackages() const
{
	return this->m_clipPackages;
}

inline QUrl fairytale::clipsDir() const
{
	return this->m_clipsDir;
}

inline bool fairytale::isPaused() const
{
	return this->m_paused;
}

inline bool fairytale::isRunning() const
{
	return this->m_isRunning;
}

#endif // fairytale_H
