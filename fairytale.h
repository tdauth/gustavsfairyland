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

		void about();

	public:
		fairytale();
		virtual ~fairytale();

		void playFinalClip(int index);

		void gameOver();

	private slots:
		void finishNarrator(QMediaPlayer::State state);
		void timerTick();

		void clickCard();

	private:
		void updateTimeLabel();
		void addCurrentSolution();
		void fillCurrentClips();
		void selectRandomSolution();

		bool loadClipsFromFile(const QString &file);

		Player *m_player;

		QTimer m_timer;
		long int m_remainingTime;

		Clip *m_currentSolution;
		QVector<QPushButton*> m_buttons;
		QVector<Clip*> m_currentClips;
		QList<Clip*> m_clips;

		QList<Clip*> m_completeSolution;
		QList<QPushButton*> m_completeSolutionButtons;
		int m_completeSolutionIndex;
		bool m_playCompleteSolution;
};

#endif // fairytale_H
