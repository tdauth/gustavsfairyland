#include <QtNetwork>

class Handler : public QObject
{
	Q_OBJECT

	public slots:
		void handleConnection();
};

void Handler::handleConnection()
{
	qDebug() << "Handle connection!";
}

int main()
{
	Handler handler;
	QTcpServer tcpServer;
	QObject::connect(&tcpServer, &QTcpServer::newConnection, &handler, &Handler::handleConnection);
	tcpServer.listen(QHostAddress::Any, 8000);

	return 1;
}

#include "server.moc"