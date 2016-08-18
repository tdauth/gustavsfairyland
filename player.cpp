#include <QtGui>
#include <QtMultimedia/QMultimedia>
#include <QShortcut>

#include "player.h"
#include "fairytale.h"
#include "iconlabel.h"

Player::Player(QWidget *parent, fairytale *app) : QDialog(parent), m_app(app), m_iconLabel(new IconLabel(this)), m_skipped(false), m_isPrefix(false)
#ifdef Q_OS_ANDROID
, m_view(nullptr), m_item(nullptr), m_mediaPlayer(nullptr), m_videoWidget(nullptr)
#else
, m_videoWidget(new QVideoWidget(this)), m_mediaPlayer(new QMediaPlayer(this))
#endif
{
	setupUi(this);
	this->setModal(true);

#ifdef Q_OS_ANDROID
	//QQuickWindow *quickWindow = new QQuickWindow(QApplication::desktop()->screen());
	// On Android a QML script is used to create a media player and video output since QVideoWidget is not supported. The script is packed as resource.
	m_view = new QQuickView(QUrl("qrc:/videoplayer.qml"));

	m_videoWidget = QWidget::createWindowContainer(m_view, this);
	m_videoWidget->setMinimumSize(200, 200);
	m_videoWidget->setMaximumSize(200, 200);
	m_videoWidget->setFocusPolicy(Qt::TabFocus);
	videoPlayerLayout->addWidget(m_videoWidget);

	qDebug() << "QML Errors:";
	foreach (QQmlError error, m_view->errors())
	{
		qDebug() << error.description();
	}

	if (m_view != nullptr)
	{
		m_item = m_view->rootObject();
		qDebug() << "Children size: " << m_item->children().size();

		if (m_item != nullptr)
		{
			m_mediaPlayer = m_item->findChild<QObject*>("player");
		}
	}
#else
	this->m_mediaPlayer->setVideoOutput(m_videoWidget);
	this->m_mediaPlayer->setAudioRole(QAudio::GameRole);

	// expanding makes sure that it uses the maximum possible size
	this->m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->m_videoWidget->setMinimumSize(QSize(240, 240));
	videoPlayerLayout->addWidget(m_videoWidget);
	m_videoWidget->show();
#endif
	volumeSlider->setValue(100);
	connect(volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
	this->setVolume(volumeSlider->value());

	this->m_iconLabel->setAlignment(Qt::AlignCenter);
	this->m_iconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->m_iconLabel->setMinimumSize(QSize(240, 240));
	videoPlayerLayout->addWidget(m_iconLabel);

	connect(this->skipPushButton, SIGNAL(clicked()), this, SLOT(skip()));
	connect(this->pausePushButton, SIGNAL(clicked()), app, SLOT(pauseGameAction()));
	connect(this->cancelPushButton, &QPushButton::clicked, app, &fairytale::cancelGame);
	connect(this, SIGNAL(rejected()), this, SLOT(skip()));

	// Global shortcuts in the widget are required since the push buttons would have to be in focus to work with shortcuts.
	QShortcut *shortcut = new QShortcut(QKeySequence("SPACE"), this);
	connect(shortcut, &QShortcut::activated, this, &Player::skip);
	shortcut = new QShortcut(QKeySequence("P"), this);
	connect(shortcut, &QShortcut::activated, app, &fairytale::pauseGameAction);
	shortcut = new QShortcut(QKeySequence("C"), this);
	connect(shortcut, &QShortcut::activated, app, &fairytale::cancelGame);
}

Player::~Player()
{
#ifdef Q_OS_ANDROID
	if (m_view != nullptr)
	{
		delete m_view;
	}
#endif
}

QMediaPlayer::State Player::state() const
{
#ifdef Q_OS_ANDROID
	const int state = QQmlProperty::read(m_mediaPlayer, "playbackState").toInt();
	/*
	 QMediaPlayer::StoppedState	0	The media player is not playing content, playback will begin from the start of the current track.
	 QMediaPlayer::PlayingState	1	The media player is currently playing content.
	 QMediaPlayer::PausedState	2

	 Note that QML has a different order of states:
	 PlayingState - the media is currently playing.
	 PausedState - playback of the media has been suspended.
	 StoppedState - playback of the media is yet to begin.
	*/

	qDebug() << "Has state:" << state;

	return QMediaPlayer::State(state);
	/*
	switch (state)
	{
		case 0:
			return QMediaPlayer::PlayingState;

		case 1:
			return QMediaPlayer::PausedState;

		case 2:
			return QMediaPlayer::StoppedState;
	}

	qDebug() << "Unknown state:" << state;

	throw state;
	*/
#else
	return this->mediaPlayer()->state();
#endif
}

int Player::volume() const
{
#ifdef Q_OS_ANDROID
	const qreal volume = QQmlProperty::read(m_mediaPlayer, "volume").toReal();

	return volume * 100;
#else
	return this->mediaPlayer()->volume();
#endif
}

void Player::playVideo(fairytale *app, const QUrl &url, const QString &description)
{
	this->m_isPrefix = false;
	this->m_skipped = false;
	this->m_iconLabel->hide();
	this->m_iconLabel->setFile("");
	this->m_videoWidget->show();

	if (app->isFullScreen())
	{
		this->showFullScreen();
	}
	else
	{
		this->show();
	}

	this->skipPushButton->setEnabled(true);
	this->skipPushButton->setFocus();
	this->cancelPushButton->show();
	this->pausePushButton->show();

	this->descriptionLabel->setText(description);
	const QUrl resolvedUrl = app->resolveClipUrl(url);

#ifdef Q_OS_ANDROID
	m_view->show();

	if (m_mediaPlayer)
	{
		qDebug() << "There is a player and we set its source to:" << resolvedUrl;
		m_mediaPlayer->setProperty("source", resolvedUrl);
		QQmlProperty::write(m_mediaPlayer, "source", resolvedUrl);
	}
	else
	{
		qDebug() << "No player!";
	}
#else
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(resolvedUrl);
#endif

	this->play();
}

void Player::playBonusVideo(fairytale *app, const QUrl &url, const QString &description)
{
	this->playVideo(app, url, description);
	// Bonus videos are not played during a game.
	this->cancelPushButton->hide();
	this->pausePushButton->hide();
}

void Player::playSound(fairytale *app, const QUrl &url, const QString &description, const QUrl &imageUrl, bool prefix)
{
	const QUrl resolvedImageUrl = app->resolveClipUrl(imageUrl);
#ifndef Q_OS_ANDROID
	const QString imageFile = resolvedImageUrl.toLocalFile();
#else
	const QString imageFile = resolvedImageUrl.url();
#endif
	qDebug() << "Image file:" << imageFile;
	const QUrl soundUrl = app->resolveClipUrl(url);

	this->m_isPrefix = prefix;
	this->m_skipped = false;
	this->m_videoWidget->hide();
	this->m_iconLabel->show();
	this->m_iconLabel->setFile(imageFile);

	if (app->isFullScreen())
	{
		this->showFullScreen();
	}
	else
	{
		this->show();
	}

	this->skipPushButton->setEnabled(true);
	this->skipPushButton->setFocus();
	this->cancelPushButton->show();
	this->pausePushButton->show();

	this->descriptionLabel->setText(description);

#ifdef Q_OS_ANDROID
	if (m_mediaPlayer)
	{
		qDebug() << "There is a player and we set its source to:" << soundUrl;
		m_mediaPlayer->setProperty("source", soundUrl);
		QQmlProperty::write(m_mediaPlayer, "source", soundUrl);
	}
	else
	{
		qDebug() << "No player!";
	}
#else
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(soundUrl);
	this->m_mediaPlayer->play();
#endif
}

void Player::play()
{
#ifdef Q_OS_ANDROID
	qDebug() << "Play on Android by invoking the QML method";
	QMetaObject::invokeMethod(m_mediaPlayer, "play");
#else
	this->mediaPlayer()->play();
#endif
}

void Player::pause()
{
#ifdef Q_OS_ANDROID
	QMetaObject::invokeMethod(m_mediaPlayer, "pause");
#else
	this->mediaPlayer()->pause();
#endif
}

void Player::stop()
{
#ifdef Q_OS_ANDROID
	qDebug() << "Invoking stop method";
	QMetaObject::invokeMethod(m_mediaPlayer, "stop");
#else
	this->mediaPlayer()->stop();
#endif
}

void Player::setVolume(int volume)
{
#ifdef Q_OS_ANDROID
	const qreal realVolume = (qreal)(volume) * 0.01;
	m_mediaPlayer->setProperty("volume", realVolume);
	QQmlProperty::write(m_mediaPlayer, "volume", realVolume);

	qDebug() << "Set volume to" << realVolume;
#else
	this->mediaPlayer()->setVolume(volume);
#endif
}

void Player::skip()
{
	if (this->m_skipped)
	{
		qDebug() << "Invalid skip call";

		return;
	}

	this->m_skipped = true;
	this->hide();

	// stop after hiding since stopping might lead to showing it again
	this->stop();
}
