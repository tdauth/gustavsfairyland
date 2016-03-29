#ifndef GAMEMODEMOVING_H
#define GAMEMODEMOVING_H

#include "gamemode.h"

class RoomWidget;

/**
 * \brief In this game mode every round one solution is randomly chosen. This clip button moves around in a \ref RoomWidget and has to be selected by the user in time.
 */
class GameModeMoving : public GameMode
{
	public:
		GameModeMoving(fairytale *app);

		virtual GameMode::State state() override;
		virtual Clip* solution() override;
		virtual long int time() override;
		virtual void afterNarrator() override;
		virtual void nextTurn() override;
		virtual void resume() override;
		virtual void pause() override;
		virtual void end() override;
		virtual void start() override;

	private:
		State m_state;
		QList<Clip*> m_remaining;
		Clip *m_currentSolution;
		RoomWidget *m_roomWidget;
};

#endif // GAMEMODEMOVING_H
