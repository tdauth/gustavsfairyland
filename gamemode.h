#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QtCore/QObject>

class fairytale;
class Clip;

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

		virtual void start() = 0;
		virtual void end() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual void nextTurn() = 0;
		virtual void afterNarrator() = 0;
		virtual long int time() = 0;
		virtual Clip* solution() = 0;
		virtual State state() = 0;

	private:
		fairytale *m_app;
};

inline fairytale* GameMode::app() const
{
	return this->m_app;
}

#endif // GAMEMODE_H
