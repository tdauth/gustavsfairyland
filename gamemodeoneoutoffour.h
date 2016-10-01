#ifndef GAMEMODEONEOUTOFFOUR_H
#define GAMEMODEONEOUTOFFOUR_H

#include <QtCore/QVector>
#include <QtWidgets/QPushButton>

#include "gamemode.h"

class IconButton;

/**
 * \brief Provides a very basic game mode where the player has to choose one proper clip from a number of shown clips.
 */
class GameModeOneOutOfFour : public GameMode
{
	Q_OBJECT

	public slots:
		void clickCard();

	public:
		GameModeOneOutOfFour(fairytale *app, int size = 4);

		/**
		 * \return Returns the maximum number of shown clips per round.
		 */
		int size() const;

		virtual QString id() const override;
		virtual QString name() const override;
		virtual void start() override;
		virtual void end() override;
		virtual void pause() override;
		virtual void resume() override;
		virtual GameMode::State state() override;
		virtual Clip* solution() override;
		virtual void nextTurn() override;
		virtual void afterNarrator() override;
		virtual bool hasToChooseTheSolution() override;
		virtual bool hasLimitedTime() override;
		virtual long int time() override;

	private:
		void fillCurrentClips();
		void selectRandomSolution();
		void setState(State state);

		/**
		 * The maximum number of possible clips to choose in one round.
		 */
		int m_size;
		/**
		 * All clips which have not yet been a solution in one round.
		 */
		QList<Clip*> m_remainingClips;
		/**
		 * The current solution of this round which has to be selected.
		 */
		Clip *m_currentSolution;
		/**
		 * All clip buttons from which the user has to choose one solution.
		 */
		QVector<IconButton*> m_buttons;
		QVector<Clip*> m_currentClips;
		/**
		 * The current state of the game mode.
		 */
		State m_state;
};

inline QString GameModeOneOutOfFour::id() const
{
	return "onepageoutoffour";
}

inline QString GameModeOneOutOfFour::name() const
{
	return tr("One Page Out Of Four");
}

inline int GameModeOneOutOfFour::size() const
{
	return this->m_size;
}

inline void GameModeOneOutOfFour::setState(GameMode::State state)
{
	this->m_state = state;
}

inline bool GameModeOneOutOfFour::hasToChooseTheSolution()
{
	return true;
}

inline bool GameModeOneOutOfFour::hasLimitedTime()
{
	return true;
}

#endif // GAMEMODEONEOUTOFFOUR_H
