#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QObject>

#include "fairytale.h"

/**
 * \brief Abstract game mode class for implementing custom game modes.
 *
 * The game mode is choosen by the player when the game is started.
 */
class GameMode : public QObject
{
	public:
		/**
		 * \brief Every game mode has a current state while being played.
		 * All possible states of a game mode.
		 */
		enum class State
		{
			None,
			Won,
			Running,
			Lost
		};

		GameMode(fairytale *app);
		virtual ~GameMode();

		/**
		 * \return Returns the corresponding application of the game mode.
		 */
		fairytale* app() const;

		/**
		 * Every game mode can be identified by a unique ID. This is useful for storing unique information corresponding to a game mode like highscores
		 * and not depending on the game mode's name which might change when change the locale settings.
		 * \return Returns a unique ID.
		 */
		virtual QString id() const = 0;
		/**
		 * Every game mode has a name which is shown to the player when he/she chooses a game mode.
		 * \return Returns the readable name of the game mode.
		 */
		virtual QString name() const = 0;
		virtual void start() = 0;
		virtual void end() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual void nextTurn() = 0;
		virtual void afterNarrator() = 0;
		virtual long int time() = 0;
		virtual fairytale::ClipKey solution() = 0;
		virtual State state() = 0;

		/**
		 * \return Returns true if the game mode sets a solution in \ref nextTurn() which has to be chosen in \ref afterNarrator(). Otherwise it returns false.
		 */
		virtual bool hasToChooseTheSolution();
		virtual bool playIntro();
		virtual bool playOutro();
		virtual bool useMaxRounds();
		virtual bool useDifficulty();
		virtual bool hasLimitedTime();
		virtual bool showWinDialog();
		virtual bool unlockBonusClip();
		virtual bool addToHighScores();

	private:
		fairytale *m_app;
};

inline fairytale* GameMode::app() const
{
	return this->m_app;
}

inline bool GameMode::hasToChooseTheSolution()
{
	return true;
}

inline bool GameMode::playIntro()
{
	return true;
}

inline bool GameMode::playOutro()
{
	return true;
}

inline bool GameMode::useMaxRounds()
{
	return true;
}

inline bool GameMode::useDifficulty()
{
	return true;
}

inline bool GameMode::hasLimitedTime()
{
	return true;
}

inline bool GameMode::showWinDialog()
{
	return true;
}

inline bool GameMode::unlockBonusClip()
{
	return true;
}

inline bool GameMode::addToHighScores()
{
	return true;
}

#endif // GAMEMODE_H
