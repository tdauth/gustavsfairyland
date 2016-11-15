#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QRunnable>
#include <QTcpSocket>

class myHTTPserver;

class RequestHandler : public QRunnable
{
	public:
		RequestHandler(myHTTPserver *server, QTcpSocket *socket);
		virtual void run() override;

		void handleRequest(QTcpSocket *socket, const QByteArray &data);
		void sendReply(QTcpSocket *socket, const QByteArray &data);

	private:
		myHTTPserver *server;
		QTcpSocket *socket;
};

#endif
