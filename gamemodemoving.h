#ifndef GAMEMODEMOVING_H
#define GAMEMODEMOVING_H

#include "gamemode.h"

class RoomWidget;

/**
 * \brief In this game mode every round one solution is randomly chosen. This clip button moves around in a \ref RoomWidget and has to be selected by the user in time.
 */
class GameModeMoving : public GameMode
{
	Q_OBJECT

	public:
		GameModeMoving(fairytale *app);

		virtual QString name() const override;
		virtual GameMode::State state() override;
		virtual Clip* solution() override;
		virtual long int time() override;
		virtual void afterNarrator() override;
		virtual void nextTurn() override;
		virtual void resume() override;
		virtual void pause() override;
		virtual void end() override;
		virtual void start() override;

		int startSpeed() const;

	private slots:
		void gotIt();
		void lost();

	private:
		void setState(State state);

		State m_state;
		/**
		 * All clips which have not yet been a solution in one round.
		 */
		QList<Clip*> m_remainingClips;
		Clip *m_currentSolution;
		RoomWidget *m_roomWidget;
};

inline QString GameModeMoving::name() const
{
	return tr("Pages On The Ground");
}

inline void GameModeMoving::setState(GameMode::State state)
{
	this->m_state = state;
}

#endif // GAMEMODEMOVING_H
