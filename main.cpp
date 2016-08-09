#include "fairytale.h"

#include <QtGlobal>
#include <QtDebug>
#include <QtWidgets/QApplication>
#include <QTime>

#include <stdio.h>
#include <stdlib.h>

void myMessageOutput(QtMsgType type, const QMessageLogContext &c, const QString &msg)
{
	//in this function, you can write the message to any stream!
	switch (type) {
		case QtInfoMsg:
			fprintf(stderr, "Info: %s\n", msg.toUtf8().constData());
			break;

		case QtDebugMsg:
			fprintf(stderr, "Debug: %s\n", msg.toUtf8().constData());
			break;

		case QtWarningMsg:
			fprintf(stderr, "Warning: %s\n", msg.toUtf8().constData());
			break;

		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s\n", msg.toUtf8().constData());
			break;

		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s\n", msg.toUtf8().constData());
			abort();

		default:
			fprintf(stderr, "Unknown: %s\n", msg.toUtf8().constData());
			break;
	}
}

int main(int argc, char** argv)
{
	qInstallMessageHandler(myMessageOutput); //install : set the callback
	QApplication app(argc, argv);

	// Create seed for the random
	// That is needed only once on application startup
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	fairytale fairytale;
	// TODO in Fullscreen mode on Windows 7 repaint() does not cause immediate paintEvent() call! Works only when showing it and then calling showFullScreen().
	fairytale.show();
	fairytale.showFullScreen();

	return app.exec();
}
