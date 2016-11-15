#include <QtCore>

#include "requesthandler.h"
#include "httpserver.h"

RequestHandler::RequestHandler(myHTTPserver* server, QTcpSocket *socket) : server(server), socket(socket)
{
}

void RequestHandler::run()
{
	while (!(socket->waitForReadyRead(100)));  //waiting for data to be read from web browser

	const QByteArray data = socket->readAll();
	handleRequest(socket, data);

	socket->flush();
	socket->disconnectFromHost();
}

void RequestHandler::handleRequest(QTcpSocket *socket, const QByteArray &data)
{
	qDebug() << "Got request:" << data;
	int nextId = server->getNextId();

	const QByteArray createCommand = "GET /create";

	if (data.startsWith("GET /list"))
	{
		const QByteArray reply = QByteArray(server->games().join("\n").toUtf8());
		sendReply(socket, reply);
	}
	else if (data.startsWith(createCommand))
	{
		const QByteArray reply = QString::number(nextId).toUtf8();
		sendReply(socket, reply);
		server->addGame(QString::number(nextId));
		nextId += 1;
	}
	else if (data.startsWith("GET /waitforclip"))
	{
		const QString clip = server->waitForClip();
		// TODO make file available
		const QByteArray reply = clip.toUtf8();
		sendReply(socket, reply);
	}
	else
	{
		sendReply(socket, QByteArray("Unknown command"));
	}
}

void RequestHandler::sendReply(QTcpSocket *socket, const QByteArray &data)
{
	socket->write("HTTP/1.1 200 OK\r\n");       // \r needs to be before \n
	socket->write("Content-Type: text/html\r\n");
	socket->write("Connection: close\r\n");

	socket->write(data);

	qDebug() << "Sending reply:" << data;
}
