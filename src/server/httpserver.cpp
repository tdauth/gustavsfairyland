#include <iostream>

#include <QtNetwork>

#include "httpserver.h"
#include "requesthandler.h"

myHTTPserver::myHTTPserver(QObject *parent) : QObject(parent)
{
	server = new QTcpServer(this);
	// waiting for the web brower to make contact,this will emit signal
	connect(server, SIGNAL(newConnection()),this, SLOT(myConnection()));

	if (!server->listen(QHostAddress::Any, 8081))
	{
		std::cout<< "\nWeb server      could not start";
	}
	else
	{
		std::cout<<"\nWeb server is waiting for a connection on port 8081";
		m_threadPool.setMaxThreadCount(5);
		m_threadPool.waitForDone();
		std::cout << "Finished thread pool\n";
	}
}

void myHTTPserver::myConnection()
{
	QTcpSocket *socket = server->nextPendingConnection();

	// TODO threadsafe
	m_socketsMutex.lock();
	sockets.push_back(socket);
	connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
	m_socketsMutex.unlock();

	RequestHandler *runnable = new RequestHandler(this, socket);
	m_threadPool.start(runnable);
}
myHTTPserver::~myHTTPserver()
{
	m_socketsMutex.lock();

	foreach (QTcpSocket *socket, sockets)
	{
		socket->close();
	}

	m_socketsMutex.unlock();
}

QStringList myHTTPserver::games()
{
	m_gamesMutex.lock();
	const QStringList result = m_games;
	m_gamesMutex.unlock();

	return result;
}

void myHTTPserver::addGame(const QString& game)
{
	m_gamesMutex.lock();
	m_games.push_back(game);
	m_gamesMutex.unlock();
}

int myHTTPserver::getNextId()
{
	int result = m_nextGameId;

	m_nextGameId++;

	return result;
}

QString myHTTPserver::waitForClip()
{
	while (true)
	{
		m_clipMutex.lock();

		if (m_clipFile.isEmpty())
		{
			m_clipWaitCondition.wait(&m_clipMutex);
		}
		else
		{
			m_clipMutex.unlock();

			return m_clipFile;
		}
	}

	return "";
}

#include "httpserver.moc"
