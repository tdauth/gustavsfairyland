#include "fairytale.h"

#include <QtGlobal>
#include <QtDebug>
#include <QSplashScreen>
#include <QtWidgets/QApplication>
#include <QTime>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	// Create seed for the random
	// That is needed only once on application startup
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	QSplashScreen splash;
	splash.setPixmap(QPixmap(":/resources/splashscreen.jpg").scaled(256, 256, Qt::KeepAspectRatio));
	splash.show();
	app.processEvents();

	fairytale fairytale;
	// TODO in Fullscreen mode on Windows 7 repaint() does not cause immediate paintEvent() call! Works only when showing it and then calling showFullScreen().
	fairytale.show();
	fairytale.showFullScreen();

	splash.finish(&fairytale);

	return app.exec();
}
