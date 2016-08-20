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
	// Support better scaling on Android.
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QApplication app(argc, argv);

	// Create seed for the random
	// That is needed only once on application startup
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

// Dont use splash screen: https://forum.qt.io/topic/60795/qt-qml-android-black-screen-when-i-run-app-on-device
#ifndef Q_OS_ANDROID
	QSplashScreen splash;
	splash.setPixmap(QPixmap(":/resources/splashscreen.jpg").scaled(256, 256, Qt::KeepAspectRatio));
	splash.show();
	app.processEvents();
#endif

	fairytale fairytale;
	// TODO in Fullscreen mode on Windows 7 repaint() does not cause immediate paintEvent() call! Works only when showing it and then calling showFullScreen().
	fairytale.show();

// showing maximized or fullscren leads to menu actions disappearing on a smartphone
#ifndef Q_OS_ANDROID
	fairytale.showFullScreen();
	splash.finish(&fairytale);
#endif

	return app.exec();
}
