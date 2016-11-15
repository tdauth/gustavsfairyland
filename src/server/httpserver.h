#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QList>
#include <QThreadPool>
#include <QMutex>
#include <QWaitCondition>
#include <QStringList>

class myHTTPserver : public QObject
{
	Q_OBJECT

	public:
		explicit myHTTPserver(QObject *parent = 0);
		~myHTTPserver();

	public slots:
		void myConnection();

		/**
		 * TODO Threadsafe
		 */
		QStringList games();
		void addGame(const QString &game);
		int getNextId();
		QString waitForClip();

	private:
		qint64 bytesAvailable() const;

		QTcpServer *server;
		/**
		 * TODO threadsafe
		 */
		QMutex m_socketsMutex;
		QList<QTcpSocket*> sockets;

		QThreadPool m_threadPool;

		QMutex m_clipMutex;
		QWaitCondition m_clipWaitCondition;
		QString m_clipFile;

		QMutex m_gamesMutex;
		QStringList m_games;
		QAtomicInt m_nextGameId;
};

#endif
