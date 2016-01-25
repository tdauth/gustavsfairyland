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

class fairytale : public QMainWindow, protected Ui::MainWindow
{
	Q_OBJECT

	public slots:
		void newGame();
		void nextTurn();
		void clear();
		void clearSolution();
		void clearClips();

		void playFinalVideo();

		void pauseGame();
		void openClipsDialog();
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

	private slots:
		void finishNarrator(QMediaPlayer::State state);
		void timerTick();

		void clickCard();

	private:
		void updateTimeLabel();
		void addCurrentSolution();
		void fillCurrentClips();
		void selectRandomSolution();


		QUrl m_clipsDir;

		int m_turns;

		Clip *m_startPerson;

		Player *m_player;

		ClipsDialog *m_clipsDialog;
		ClipPackageDialog *m_clipPackageDialog;

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

#endif // fairytale_H
