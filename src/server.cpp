#include <QtNetwork>

int main()
{
	//QNetworkAccessManager networkManager;
	//networkManager.
	QTcpServer tcpServer;
	tcpServer.listen(QHostAddress::Any, 8000);



	return 1;
}