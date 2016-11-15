#ifndef GAMEMODESTORY_H
#define GAMEMODESTORY_H

#include <QNetworkAccessManager>

#include "gamemode.h"
#include "recorder.h"

/**
 * \brief A multiplayer game mode where every play makes his own clips and sends them to the other players.
 */
class GameModeStory : public GameMode
{
	Q_OBJECT

	public:
		GameModeStory(fairytale *app);

		virtual GameMode::State state() override;
		virtual Clip* solution() override;
		virtual long int time() override;
		virtual void afterNarrator() override;
		virtual void nextTurn() override;
		virtual void resume() override;
		virtual void pause() override;
		virtual void end() override;
		virtual void start() override;
		virtual QString name() const override;
		virtual QString id() const override;

		virtual bool hasToChooseTheSolution() override;
		virtual bool hasLimitedTime() override;
		virtual bool playIntro() override;
		virtual bool playOutro() override;
		virtual bool useMaxRounds() override;
		virtual bool useDifficulty() override;
		virtual bool showWinDialog() override;
		virtual bool unlockBonusClip() override;
		virtual bool addToHighScores() override;

	private slots:
		void onVideoRecorderStateChanged(QMediaRecorder::State state);
		void onAudioRecorderStateChanged(QMediaRecorder::State state);
		void onImageCaptured(int id, const QString &fileName);

		void continueGameMode();
		void finishGameMode();

	private:
		void setState(State state);
		QNetworkAccessManager* networkAccessManager() const;

		State m_state;
		Clip *m_currentSolution;
		Recorder *m_recorder;

		QNetworkAccessManager *m_networkAccessManager;

		QPushButton *m_continueButton;
		QPushButton *m_finishButton;
};

inline QNetworkAccessManager* GameModeStory::networkAccessManager() const
{
	return this->m_networkAccessManager;
}

#endif // GAMEMODESTORY_H
