#include <QtNetwork>
#include <QtWidgets>

#include "gamemodemultiplayer.h"

GameModeMultiplayer::GameModeMultiplayer(fairytale* app): GameMode(app), m_state(State::None)
{
}

GameMode::State GameModeMultiplayer::state()
{
	return this->m_state;
}

GameMode::ClipKeys GameModeMultiplayer::solutions()
{
	ClipKeys result;

	return result;
}

long int GameModeMultiplayer::time()
{
	return 0;
}

void GameModeMultiplayer::afterNarrator()
{
}

void GameModeMultiplayer::nextTurn()
{
}

void GameModeMultiplayer::resume()
{
}

void GameModeMultiplayer::pause()
{
}

void GameModeMultiplayer::end()
{
	QNetworkRequest endRequest(QUrl("http://wc3lib.org/gustavsfairyland/end"));
	QNetworkReply *reply = m_networkManager.post(endRequest, QByteArray("id=") + this->m_gameId + QByteArray("\ntoken=") + this->m_gameToken);
	const int result = waitForReply(reply);

	if (result >= 200 && result <= 300)
	{
		qDebug() << "Success";
		qDebug() << "Reply:" << reply->readAll();
	}
	else
	{
		qDebug() << "Error:" << result;
	}
}

void GameModeMultiplayer::start()
{
	QNetworkRequest startRequest(QUrl("http://wc3lib.org/gustavsfairyland/start"));
	QNetworkReply *reply = m_networkManager.get(startRequest);
	const int result = waitForReply(reply);

	if (result >= 200 && result <= 300)
	{
		qDebug() << "Success";
		qDebug() << "Reply:" << reply->readAll();
	}
	else
	{
		this->setState(State::Lost);
		qDebug() << "Error:" << result;

		QMessageBox::critical(this->app(), tr("Error"), tr("HTTP error %1.").arg(result));
	}
}

int GameModeMultiplayer::waitForReply(QNetworkReply *reply)
{
	QEventLoop loop;
	QTimer timer;
	timer.setSingleShot(true);
	timer.start(10000);
	int result = -1;

	while (timer.isActive())
	{
		loop.processEvents(QEventLoop::AllEvents, 1000);

		const QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

		if (variant.isValid())
		{
			result = variant.toInt();

			break;
		}
	}

	return result;
}