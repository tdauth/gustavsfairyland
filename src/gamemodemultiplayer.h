#ifndef GAMEMODEMULTIPLAYER_H
#define GAMEMODEMULTIPLAYER_H

#include <QNetworkAccessManager>

#include "gamemode.h"

class GameModeMultiplayer : public GameMode
{
	Q_OBJECT

	public:
		GameModeMultiplayer(fairytale *app);

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

	private:
		void setState(State state);
		/**
		 * Waits for a HTTP reply with a standard timeout and returns the result HTTP code.
		 * \param reply The HTTP reply object which is waited for.
		 * \return Returns -1 if the reply has timed out. Otherwise it returns the HTTP result code.
		 */
		int waitForReply(QNetworkReply *reply);

		State m_state;
		QNetworkAccessManager m_networkManager;
		QByteArray m_gameId;
		QByteArray m_gameToken;
};


inline QString GameModeMultiplayer::id() const
{
	return "multiplayer";
}

inline QString GameModeMultiplayer::name() const
{
	return tr("Multiplayer");
}

inline void GameModeMultiplayer::setState(GameMode::State state)
{
	this->m_state = state;
}

#endif // GAMEMODEMULTIPLAYER_H