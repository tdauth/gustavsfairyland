#include "fairytale.h"

#include <QtGlobal>
#include <QSplashScreen>
#include <QApplication>
#include <QTime>

int main(int argc, char** argv)
{
	// Support better scaling on Android.
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

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

	// Loads the settings and therefore is shown.
	fairytale fairytale;

#ifndef Q_OS_ANDROID
	splash.finish(&fairytale);
#endif

	return app.exec();
}
