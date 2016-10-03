#include "fairytale.h"

#include <QtGlobal>
#include <QtDebug>
#include <QSplashScreen>
#include <QtWidgets/QApplication>
#include <QTime>

#include <stdio.h>
#include <stdlib.h>

#ifdef Q_OS_ANDROID
#include <QtQuick/QQuickView>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlProperty>
#include <QWidget>

#include <QtAV>
#include <QtAVWidgets>
#include <QMessageBox>

void playAndroidVideo(QWidget *parent)
{
	QQuickView *view = new QQuickView();
	QWidget *window = QWidget::createWindowContainer(view, parent);
	window->setFocusPolicy(Qt::TabFocus);

	// TEST to check if the widget appears at all
	QPalette pal;
	pal.setColor(QPalette::Background, Qt::green);
	window->setPalette(pal);

	view->setSource(QUrl("qrc:/videoplayer.qml"));

	qDebug() << "QML Errors:";
	foreach (QQmlError error, view->errors())
	{
		qDebug() << error.description();
	}

	view->show();
}

#endif

int main(int argc, char** argv)
{
	// Support better scaling on Android.
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QApplication app(argc, argv);

	// TEST
#ifdef Q_OS_ANDROID
	//QWidget *widget = new QWidget();
	//widget->showFullScreen();
	//playAndroidVideo(widget);


	QtAV::WidgetRenderer renderer;
	renderer.show();

	QtAV::AVPlayer *player = new QtAV::AVPlayer(&app);
	player->setRenderer(&renderer);

	const QFileInfo fileInfo("assets:/clips/mahler/bonus/trauer.mkv");

	if (fileInfo.exists())
	{
		QMessageBox::information(nullptr, QObject::tr("Exists!"), QObject::tr("Exists!"));
	}


	player->setFile("assets:/clips/mahler/bonus/trauer.mkv");
	player->load();
	// disable audio for tests
	//player->audio()->setBackends(QStringList() << "null");
	player->play();

	return app.exec();
#endif

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

// showing maximized or fullscren leads to menu actions disappearing on a smartphone
#ifndef Q_OS_ANDROID
	//fairytale.showFullScreen();
	splash.finish(&fairytale);
#endif

	return app.exec();
}
