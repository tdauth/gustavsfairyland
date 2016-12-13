#ifndef GAMEMODESEQUENCE_H
#define GAMEMODESEQUENCE_H

#include "gamemode.h"

class RoomWidget;
class SolutionWidget;

/**
 * \brief In this game mode the complete solution is predefined and has to be finished in one turn. All clip buttons move around in a \ref RoomWidget and have to be dragged & dropped by the user in time.
 */
class GameModeSequence : public GameMode
{
	Q_OBJECT

	public:
		static const int maxFloatingClips = 7;

		GameModeSequence(fairytale *app);

		virtual QString id() const override;
		virtual QString name() const override;
		virtual GameMode::State state() override;
		virtual ClipKeys solutions() override;
		virtual long int time() override;
		virtual void afterNarrator() override;
		virtual void nextTurn() override;
		virtual void resume() override;
		virtual void pause() override;
		virtual void end() override;
		virtual void start() override;

		virtual bool hasToChooseTheSolution() override;
		virtual bool useMaxRounds() override;

	private slots:
		void solved(const fairytale::ClipKey &clipKey);
		void failed(const fairytale::ClipKey &clipKey);
		void lost();

	private:
		void setState(State state);

		State m_state;
		ClipKeys m_remainingClips;
		ClipKeys m_currentSolution;
		RoomWidget *m_roomWidget;
		/**
		 * The widget where the clips have to be dropped.
		 */
		SolutionWidget *m_solutionWidget;
};

inline QString GameModeSequence::id() const
{
	return "sequence";
}

inline QString GameModeSequence::name() const
{
	return tr("Sequence");
}

inline void GameModeSequence::setState(GameMode::State state)
{
	this->m_state = state;
}

#endif // GAMEMODESEQUENCE_H
