#include <QtGui>
#include <QMultimedia>

#if defined(Q_OS_ANDROID) && defined(USE_ANDROID_JAVA_PLAYER)
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

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
#ifdef USE_QTAV
, m_player(nullptr)
, m_renderer(nullptr)
#else
, m_videoWidget(new QVideoWidget(this)), m_mediaPlayer(new QMediaPlayer(this))
#endif
{
	setupUi(this);

#ifdef USE_QTAV
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

	this->setVolume(fairytale::defaultVideoSoundVolume);

	this->m_parallelSoundsMediaPlayer->setAudioRole(QAudio::GameRole);
	connect(this->m_parallelSoundsMediaPlayer, &QMediaPlayer::stateChanged, this, &Player::onChangeStateParallelSoundPlayer);

#ifndef USE_QTAV
	connect(this->mediaPlayer(), &QMediaPlayer::stateChanged, this, &Player::onChangeState);
#else
	connect(this->m_player, &QtAV::AVPlayer::stateChanged, this, &Player::onChangeStateAndroidQtAv);
	connect(this->m_player, &QtAV::AVPlayer::error, this, &Player::onError);
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

#ifndef Q_OS_ANDROID
	connect(this, &QDialog::rejected, this, &Player::skip);
#endif

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
#ifdef USE_QTAV
	return QMediaPlayer::State(this->m_player->state());
#else
	return this->mediaPlayer()->state();
#endif
}

int Player::volume() const
{
#ifdef USE_QTAV
	return this->m_player->audio()->volume() * 100.0;
#else
	return this->mediaPlayer()->volume();
#endif
}

bool Player::isMuted() const
{
#ifdef USE_QTAV
	return this->m_player->audio()->isMute();
#else
	return this->mediaPlayer()->isMuted();
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

void Player::hideEvent(QHideEvent *event)
{
	Base::hideEvent(event);

	qDebug() << "Hide player!";
}

void Player::onChangeStateParallelSoundPlayer(QMediaPlayer::State state)
{
	Q_ASSERT(this->m_parallelSoundsMediaPlayer->state() == state);

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
			this->checkForStart();

			break;
		}

		case QMediaPlayer::PausedState:
		{
			this->checkForPause();

			break;
		}
	}
}

void Player::onChangeState(QMediaPlayer::State state)
{
	Q_ASSERT(this->state() == state);

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
			this->checkForStart();

			break;
		}

		case QMediaPlayer::PausedState:
		{
			this->checkForPause();

			break;
		}
	}
}

#ifdef USE_QTAV
void Player::onChangeStateAndroidQtAv(QtAV::AVPlayer::State state)
{
	qDebug() << "State changed on Android player" << state;
	Q_ASSERT(this->m_player->state() == state);
	qDebug() << "After assert";

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

void Player::onError(const QtAV::AVError &e)
{
	qDebug() << "QtAV error:" << e;
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
		this->hideCentral();
#endif

		if (!this->m_musicWasMutedBefore)
		{
			this->m_app->setMusicMuted(false);
		}

		emit stateChangedVideoAndSounds(QMediaPlayer::StoppedState);
	}
}

void Player::checkForStart()
{
	if (this->m_parallelSoundsMediaPlayer->state() == QMediaPlayer::PlayingState || this->state() == QMediaPlayer::PlayingState)
	{
		emit stateChangedVideoAndSounds(QMediaPlayer::PlayingState);
	}
}

void Player::checkForPause()
{
	if ((this->m_parallelSoundsMediaPlayer->state() == QMediaPlayer::PausedState && this->state() == QMediaPlayer::PausedState)
			|| ((
					this->m_parallelSoundsMediaPlayer->state() == QMediaPlayer::PausedState && this->state() == QMediaPlayer::StoppedState
					)
				|| (
					this->m_parallelSoundsMediaPlayer->state() == QMediaPlayer::StoppedState && this->state() == QMediaPlayer::PausedState
					)
				)
		)
	{
		emit stateChangedVideoAndSounds(QMediaPlayer::PausedState);
	}
}

#ifdef USE_QTAV
void Player::showCentral()
{
	// Make sure all hidden widgets have been shown before.
	if (!this->m_hiddenWidgets.isEmpty())
	{
		this->hideCentral();
	}

	// one top level window on Android for OpenGL
	this->m_hiddenWidgets = m_app->hideWidgetsInMainWindow();
	this->m_hiddenWidgets.removeAll(this); // never reshow the player
	m_app->centralWidget()->layout()->addWidget(this);
	this->show();
}

void Player::hideCentral()
{
	m_app->centralWidget()->layout()->removeWidget(this);
	m_app->showWidgetsInMainWindow(this->m_hiddenWidgets);
	this->m_hiddenWidgets.clear();
}
#endif

void Player::playVideo(const QUrl &url, const QString &description, bool duringGame, bool multipleVideos)
{
	this->m_isPrefix = false;
	this->m_skipped = false;
	this->m_skippedAll = false;
	this->m_iconLabel->hide();
	this->m_iconLabel->setFile("");
#ifndef USE_QTAV
	this->m_videoWidget->show();

	if (m_app->isFullScreen())
	{
		this->showFullScreen();
	}
	else
	{
		this->show();
	}
#else
	this->showCentral();
#endif

	this->skipPushButton->setEnabled(true);
	this->cancelPushButton->setVisible(duringGame);
	this->pausePushButton->setVisible(duringGame);
	this->skipAllPushButton->setVisible(multipleVideos);

	if (multipleVideos)
	{
		this->skipAllPushButton->setFocus();
	}
	else
	{
		this->skipPushButton->setFocus();
	}

	this->descriptionLabel->setText(description);
	const QUrl resolvedUrl = m_app->resolveClipUrl(url);

#ifdef USE_QTAV
	qDebug() << "Playing video:" << resolvedUrl.toString();
	qDebug() << "Audio backends:" << m_player->audio()->backends();
	qDebug() << "Current audio backend:" << m_player->audio()->backend();
	m_player->setFile(resolvedUrl.toString());
	m_player->load();
#else
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(resolvedUrl);
#endif

#if defined(Q_OS_ANDROID) && defined(USE_ANDROID_JAVA_PLAYER)
	qDebug() << "Calling Java method";

	if (QAndroidJniObject::isClassAvailable("gustavsfairyland/Player"))
	{
		qDebug() << "Class is available.";

		QAndroidJniObject player;
		player = QAndroidJniObject("gustavsfairyland/Player",
			"(Landroid/app/Activity;)V",
			QtAndroid::androidActivity().object<jobject>()
		);
	}
	else
	{
		qDebug() << "Class is not available.";
	}
#else
	this->play();
#endif

#ifdef USE_QTAV
	this->m_renderer->widget()->show();
#endif
}

void Player::playBonusVideo(const QUrl &url, const QString &description)
{
	// Bonus videos are not played during a game.
	this->playVideo(url, description, false, false);
}

void Player::playSound(const QUrl &url, const QString &description, const QUrl &imageUrl, bool prefix, bool duringGame)
{
	const QUrl resolvedImageUrl = m_app->resolveClipUrl(imageUrl);
#ifndef Q_OS_ANDROID
	const QString imageFile = resolvedImageUrl.toLocalFile();
#else
	const QString imageFile = resolvedImageUrl.url();
#endif
	qDebug() << "Image file:" << imageFile;
	const QUrl soundUrl = m_app->resolveClipUrl(url);

	this->m_isPrefix = prefix;
	this->m_skipped = false;
	this->m_skippedAll = false;
#ifndef USE_QTAV
	this->m_videoWidget->hide();
#else
	this->m_renderer->widget()->hide();
#endif
	this->m_iconLabel->show();
	this->m_iconLabel->setFile(imageFile);

#ifndef Q_OS_ANDROID
	if (m_app->isFullScreen())
	{
		this->showFullScreen();
	}
	else
	{
		this->show();
	}
#else
	this->showCentral();
#endif

	this->skipPushButton->setEnabled(true);
	this->skipPushButton->setFocus();
	this->cancelPushButton->setVisible(duringGame);
	this->pausePushButton->setVisible(duringGame);
	this->skipAllPushButton->setVisible(prefix);

	this->descriptionLabel->setText(description);

#ifdef USE_QTAV
	qDebug() << "Playing sound:" << soundUrl.toString();

	if (m_player->audio()->backend().isEmpty())
	{
		qDebug() << "No audio backend available for QtAV.";

		onChangeStateAndroidQtAv(QtAV::AVPlayer::StoppedState);

		return;
	}

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

void Player::showImage(const QUrl &imageUrl, const QString &description)
{
	const QUrl resolvedImageUrl = m_app->resolveClipUrl(imageUrl);
#ifndef Q_OS_ANDROID
	const QString imageFile = resolvedImageUrl.toLocalFile();
#else
	const QString imageFile = resolvedImageUrl.url();
#endif
	qDebug() << "Image file:" << imageFile;

	this->m_isPrefix = false;
	this->m_skipped = false;
	this->m_skippedAll = false;
#ifndef USE_QTAV
	this->m_videoWidget->hide();
#else
	this->m_renderer->widget()->hide();
#endif
	this->m_iconLabel->show();
	this->m_iconLabel->setFile(imageFile);

#ifndef Q_OS_ANDROID
	if (m_app->isFullScreen())
	{
		this->showFullScreen();
	}
	else
	{
		this->show();
	}
#else
	// one top level window on Android for OpenGL
	this->m_hiddenWidgets = m_app->hideWidgetsInMainWindow();
	m_app->centralWidget()->layout()->addWidget(this);
	this->show();
#endif

	this->skipPushButton->setEnabled(true);
	this->skipPushButton->setFocus();
	this->skipAllPushButton->hide();
	this->cancelPushButton->hide();
	this->pausePushButton->hide();

	this->descriptionLabel->setText(description);
}

void Player::playParallelSound(const QUrl &url)
{
	const QUrl soundUrl = m_app->resolveClipUrl(url);

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
	this->m_musicWasMutedBefore = this->m_app->isMusicMuted();
	this->m_app->setMusicMuted(true);
	// TODO wait for muted

#ifdef USE_QTAV
	const QFileInfo fileInfo(this->m_player->file());

	if (!fileInfo.exists() || !fileInfo.isReadable())
	{
		qDebug() << "File" << fileInfo.absoluteFilePath() << "does not exist!";

		return;
	}

	// TEST mute
	// disables audio output:
	//this->m_player->audio()->setBackends(QStringList() << "null");
	//this->m_player->audio()->setMute(true);
	qDebug() << "Play QtAV" << fileInfo.absoluteFilePath();
	this->m_player->play(); // FIXME crashes
	// TEST
	//onChangeStateAndroidQtAv(QtAV::AVPlayer::StoppedState);

#else
	qDebug() << "Play with volume" << this->volume() << "and is muted" << this->isMuted() << "and video player audio role" << this->mediaPlayer()->audioRole();
	this->mediaPlayer()->play();
#endif
}

void Player::pause()
{
#ifdef USE_QTAV
	this->m_player->pause(true);
#else
	this->mediaPlayer()->pause();
#endif
	this->m_parallelSoundsMediaPlayer->pause();
}

void Player::stop()
{
#ifdef USE_QTAV
	qDebug() << "Stopping Android player";
	this->m_player->stop();
#else
	this->mediaPlayer()->stop();
#endif
	this->m_parallelSoundsMediaPlayer->stop();
}

void Player::setVolume(int volume)
{
#ifdef USE_QTAV
	const qreal realVolume = (qreal)(volume) * 0.01;
	this->m_player->audio()->setVolume(realVolume);
#else
	this->mediaPlayer()->setVolume(volume);
#endif

	this->m_parallelSoundsMediaPlayer->setVolume(volume);
}

void Player::setMuted(bool muted)
{
#ifdef USE_QTAV
	this->m_player->audio()->setMute(muted);
#else
	this->mediaPlayer()->setMuted(muted);
#endif

	this->m_parallelSoundsMediaPlayer->setMuted(muted);
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
