#ifndef fairytale_H
#define fairytale_H

#include <QtGui/QMainWindow>
#include <QTimer>
#include <QVector>
#include <QtCore/QList>
#include "ui_mainwindow.h"

class Clip;

class fairytale : public QMainWindow, protected Ui::MainWindow
{
	Q_OBJECT

	public slots:
		void newGame();
		void skipNarrator();
		void nextTurn();
		void clear();
		void clearSolution();
		void clearClips();

		void playVideo(const QUrl &url);
	public:
		fairytale();
		virtual ~fairytale();

		void playFinalClip(int index);
		void playFinalVideo();

		void gameOver();

	private slots:
		void finishNarrator();
		void timerTick();

		void clickCard();

	private:
		void updateTimeLabel();
		void addCurrentSolution();
		void fillCurrentClips();
		void selectRandomSolution();

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
