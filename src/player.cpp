#include <QtGui>
#include <QtMultimedia/QMultimedia>
#include <QShortcut>

#include "player.h"
#include "fairytale.h"
#include "iconlabel.h"

Player::Player(QWidget *parent, fairytale *app)
#ifndef Q_OS_ANDROID
: QDialog(parent)
#else
: QWidget(parent)
#endif
, m_app(app), m_iconLabel(new IconLabel(this)), m_skipped(false), m_skippedAll(false), m_isPrefix(false), m_parallelSoundsMediaPlayer(new QMediaPlayer(this))
#ifdef Q_OS_ANDROID
, m_player(nullptr)
, m_renderer(nullptr)
#else
, m_videoWidget(new QVideoWidget(this)), m_mediaPlayer(new QMediaPlayer(this))
#endif
{
	setupUi(this);

#ifdef Q_OS_ANDROID
	this->m_player = new QtAV::AVPlayer(this);
	this->m_renderer = new QtAV::OpenGLWidgetRenderer(this); // GLWidgetRenderer2
	this->m_player->setRenderer(m_renderer);

	// expanding makes sure that it uses the maximum possible size
	this->m_renderer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//this->m_renderer->setMinimumSize(QSize(100, 100));

	videoPlayerLayout->addWidget(this->m_renderer->widget());
#else
	this->setModal(true);

	this->m_mediaPlayer->setVideoOutput(m_videoWidget);
	this->m_mediaPlayer->setAudioRole(QAudio::GameRole);
	// expanding makes sure that it uses the maximum possible size
	this->m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->m_videoWidget->setMinimumSize(QSize(240, 240));
	videoPlayerLayout->addWidget(m_videoWidget);
	this->m_videoWidget->show();
#endif

	volumeSlider->setValue(100);
	connect(volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
	this->setVolume(volumeSlider->value());

	this->m_parallelSoundsMediaPlayer->setAudioRole(QAudio::GameRole);
	connect(this->m_parallelSoundsMediaPlayer, &QMediaPlayer::stateChanged, this, &Player::onChangeStateParallelSoundPlayer);

#ifndef Q_OS_ANDROID
	connect(this->mediaPlayer(), &QMediaPlayer::stateChanged, this, &Player::onChangeState);
#else
	connect(this->m_player, &QtAV::AVPlayer::stateChanged, this, &Player::onChangeStateAndroidQtAv);
#endif

	this->m_iconLabel->setAlignment(Qt::AlignCenter);
	this->m_iconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#ifndef Q_OS_ANDROID
	this->m_iconLabel->setMinimumSize(QSize(240, 240));
#endif
	videoPlayerLayout->addWidget(m_iconLabel);

	connect(this->skipPushButton, &QPushButton::clicked, this, &Player::skip);
	connect(this->skipAllPushButton, &QPushButton::clicked, this, &Player::skipAll);
	connect(this->pausePushButton, &QPushButton::clicked, app, &fairytale::pauseGameAction);
	connect(this->cancelPushButton, &QPushButton::clicked, app, &fairytale::cancelGame);
	connect(this, SIGNAL(rejected()), this, SLOT(skip()));

	// Global shortcuts in the widget are required since the push buttons would have to be in focus to work with shortcuts.
	QShortcut *shortcut = new QShortcut(QKeySequence(tr("SPACE")), this);
	// They should always work when the player widget or any subwidget of the player widget has the focus.
	shortcut->setContext(Qt::WidgetWithChildrenShortcut);
	connect(shortcut, &QShortcut::activated, this, &Player::skip);
	shortcut = new QShortcut(QKeySequence(tr("P")), this);
	shortcut->setContext(Qt::WidgetWithChildrenShortcut);
	connect(shortcut, &QShortcut::activated, app, &fairytale::pauseGameAction);
	shortcut = new QShortcut(QKeySequence(tr("C")), this);
	shortcut->setContext(Qt::WidgetWithChildrenShortcut);
	connect(shortcut, &QShortcut::activated, app, &fairytale::cancelGame);
}

Player::~Player()
{
}

QMediaPlayer::State Player::state() const
{
#ifdef Q_OS_ANDROID
	return QMediaPlayer::State(this->m_player->state());
#else
	return this->mediaPlayer()->state();
#endif
}

int Player::volume() const
{
#ifdef Q_OS_ANDROID
	return this->m_player->audio()->volume() * 100.0;
#else
	return this->mediaPlayer()->volume();
#endif
}

void Player::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			//std::cerr << "Retranslate UI of player" << std::endl;
			this->retranslateUi(this);

			break;
		}

		default:
		{
			break;
		}
	}

	Base::changeEvent(event);
}

void Player::showEvent(QShowEvent *event)
{
	Base::showEvent(event);

	fairytale::updateSize(buttonsWidget);
}

void Player::onChangeStateParallelSoundPlayer(QMediaPlayer::State state)
{
	switch (state)
	{
		case QMediaPlayer::StoppedState:
		{
			if (!this->m_parallelSounds.isEmpty())
			{
				const QUrl soundUrl = this->m_parallelSounds.dequeue();
				this->m_parallelSoundsMediaPlayer->setMedia(soundUrl);
				this->m_parallelSoundsMediaPlayer->play();
			}
			else
			{
				this->checkForFinish();
			}

			break;
		}

		case QMediaPlayer::PlayingState:
		{
			break;
		}

		case QMediaPlayer::PausedState:
		{
			break;
		}
	}
}

void Player::onChangeState(QMediaPlayer::State state)
{
	qDebug() << "Emitting signal" << state;
	emit stateChanged(state);

	switch (state)
	{
		case QMediaPlayer::StoppedState:
		{
			this->checkForFinish();

			break;
		}

		case QMediaPlayer::PlayingState:
		{
			break;
		}

		case QMediaPlayer::PausedState:
		{
			break;
		}
	}
}

#ifdef Q_OS_ANDROID
void Player::onChangeStateAndroid()
{
	this->onChangeState(this->state());

}

void Player::onChangeStateAndroidQtAv(QtAV::AVPlayer::State state)
{
	qDebug() << "State changed on Android player" << state;

	switch (state)
	{
		case QtAV::AVPlayer::StoppedState:
		{
			this->onChangeState(QMediaPlayer::StoppedState);

			break;
		}

		case QtAV::AVPlayer::PlayingState: /// Start to play if it was stopped, or resume if it was paused
		{
			this->onChangeState(QMediaPlayer::PlayingState);

			break;
		}

		case QtAV::AVPlayer::PausedState:
		{
			this->onChangeState(QMediaPlayer::PausedState);

			break;
		}
	}
}
#endif

void Player::checkForFinish()
{
	qDebug() << "Check for finish:\nthis state:" << this->state();

	// Check if parallel sounds and the current media playing are finished and then emit a signal finishVideoAndSounds
	if (this->m_parallelSounds.isEmpty() && this->m_parallelSoundsMediaPlayer->state() == QMediaPlayer::StoppedState && this->state() == QMediaPlayer::StoppedState)
	{
		qDebug() << "Emitting signal";

#ifdef Q_OS_ANDROID
		m_app->centralWidget()->layout()->removeWidget(this);
		m_app->showWidgetsInMainWindow(this->m_hiddenWidgets);
#endif

		emit finishVideoAndSounds();
	}
}

void Player::playVideo(fairytale *app, const QUrl &url, const QString &description, bool duringGame)
{
	this->m_isPrefix = false;
	this->m_skipped = false;
	this->m_skippedAll = false;
	this->m_iconLabel->hide();
	this->m_iconLabel->setFile("");
#ifndef Q_OS_ANDROID
	this->m_videoWidget->show();

	if (app->isFullScreen())
	{
		this->showFullScreen();
	}
	else
	{
		this->show();
	}
#else
	// one top level window on Android for OpenGL
	this->m_hiddenWidgets = app->hideWidgetsInMainWindow();
	app->centralWidget()->layout()->addWidget(this);
	this->show();
#endif

	this->skipPushButton->setEnabled(true);
	this->skipPushButton->setFocus();
	this->cancelPushButton->setVisible(duringGame);
	this->pausePushButton->setVisible(duringGame);

	this->descriptionLabel->setText(description);
	const QUrl resolvedUrl = app->resolveClipUrl(url);

#ifdef Q_OS_ANDROID
	qDebug() << "Playing video:" << resolvedUrl.toString();
	m_player->setFile(resolvedUrl.toString());
	m_player->load();
	//this->m_renderer->resize(app->size() / 1.5); // make videos bigger
#else
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(resolvedUrl);
#endif

	this->play();

#ifdef Q_OS_ANDROID
	this->m_renderer->widget()->show();
#endif
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
	this->m_skippedAll = false;
#ifndef Q_OS_ANDROID
	this->m_videoWidget->hide();
#else
	this->m_renderer->widget()->hide();
#endif
	this->m_iconLabel->show();
	this->m_iconLabel->setFile(imageFile);

#ifndef Q_OS_ANDROID
	if (app->isFullScreen())
	{
		this->showFullScreen();
	}
	else
	{
		this->show();
	}
#else
	// one top level window on Android for OpenGL
	this->m_hiddenWidgets = app->hideWidgetsInMainWindow();
	app->centralWidget()->layout()->addWidget(this);
	this->show();
#endif

	this->skipPushButton->setEnabled(true);
	this->skipPushButton->setFocus();
	this->cancelPushButton->show();
	this->pausePushButton->show();

	this->descriptionLabel->setText(description);

#ifdef Q_OS_ANDROID
	qDebug() << "Playing video:" << soundUrl.toString();
	m_player->setFile(soundUrl.toString());
	m_player->load();
#else
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(soundUrl);
	//qDebug() << "Current source after setting it in QML:" << QQmlProperty::read(m_mediaPlayer, "playbackState").toInt();
#endif

	this->play();
}

void Player::playParallelSound(fairytale *app, const QUrl &url)
{
	const QUrl soundUrl = app->resolveClipUrl(url);

	if (this->m_parallelSoundsMediaPlayer->state() != QMediaPlayer::StoppedState)
	{
		this->m_parallelSounds.enqueue(soundUrl);
	}
	else
	{
		this->m_parallelSoundsMediaPlayer->setMedia(soundUrl);
		this->m_parallelSoundsMediaPlayer->play();
	}
}

void Player::play()
{
#ifdef Q_OS_ANDROID
	this->m_player->play();
#else
	this->mediaPlayer()->play();
#endif
}

void Player::pause()
{
#ifdef Q_OS_ANDROID
	this->m_player->pause(true);
#else
	this->mediaPlayer()->pause();
#endif
}

void Player::stop()
{
#ifdef Q_OS_ANDROID
	this->m_player->stop();
#else
	this->mediaPlayer()->stop();
#endif
}

void Player::setVolume(int volume)
{
#ifdef Q_OS_ANDROID
	const qreal realVolume = (qreal)(volume) * 0.01;
	this->m_player->audio()->setVolume(realVolume);
#else
	this->mediaPlayer()->setVolume(volume);
#endif

	this->m_parallelSoundsMediaPlayer->setVolume(volume);
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
	this->m_parallelSounds.clear();
	this->m_parallelSoundsMediaPlayer->stop();
}

void Player::skipAll()
{
	if (this->m_skipped)
	{
		qDebug() << "Invalid skip all call";

		return;
	}

	this->m_skipped = true;
	this->m_skippedAll = true;
	this->hide();

	// stop after hiding since stopping might lead to showing it again
	this->stop();
	this->m_parallelSounds.clear();
	this->m_parallelSoundsMediaPlayer->stop();
}