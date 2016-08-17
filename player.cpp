#include <QtGui>
#include <QtMultimedia/QMultimedia>
#include <QShortcut>

#include "player.h"
#include "fairytale.h"
#include "iconlabel.h"

Player::Player(QWidget *parent, fairytale *app) : QDialog(parent), m_app(app), m_videoWidget(new QVideoWidget(this)), m_mediaPlayer(new QMediaPlayer(this)), m_iconButton(new IconLabel(this)), m_skipped(false), m_isPrefix(false)
{
	setupUi(this);
	this->setModal(true);

	this->m_mediaPlayer->setVideoOutput(m_videoWidget);

	// expanding makes sure that it uses the maximum possible size
	this->m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->m_videoWidget->setMinimumSize(QSize(240, 240));
	videoPlayerLayout->addWidget(m_videoWidget);
	m_videoWidget->show();

	this->m_iconButton->setAlignment(Qt::AlignCenter);
	this->m_iconButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->m_iconButton->setMinimumSize(QSize(240, 240));
	videoPlayerLayout->addWidget(m_iconButton);

	volumeSlider->setValue(this->m_mediaPlayer->volume());
	connect(volumeSlider, SIGNAL(valueChanged(int)), this->m_mediaPlayer, SLOT(setVolume(int)));
	this->m_mediaPlayer->setVolume(volumeSlider->value());

	connect(this->skipPushButton, SIGNAL(clicked()), this, SLOT(skip()));
	connect(this->pausePushButton, SIGNAL(clicked()), app, SLOT(pauseGameAction()));
	connect(this->cancelPushButton, &QPushButton::clicked, app, &fairytale::cancelGame);
	connect(this, SIGNAL(rejected()), this, SLOT(skip()));

	// Global shortcuts in the widget are required since the push buttons would have to be in focus to work with shortcuts.
	QShortcut *shortcut = new QShortcut(QKeySequence("SPACE"), this);
	connect(shortcut, &QShortcut::activated, this->skipPushButton, &QPushButton::click);
	shortcut = new QShortcut(QKeySequence("P"), this);
	connect(shortcut, &QShortcut::activated, this->pausePushButton, &QPushButton::click);
	shortcut = new QShortcut(QKeySequence("C"), this);
	connect(shortcut, &QShortcut::activated, this->cancelPushButton, &QPushButton::click);
}

void Player::playVideo(fairytale *app, const QUrl& url, const QString &description)
{
	this->m_isPrefix = false;
	this->m_skipped = false;
	this->m_iconButton->hide();
	this->m_iconButton->setFile("");
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
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(app->resolveClipUrl(url));
	this->m_mediaPlayer->play();
}

void Player::playBonusVideo(fairytale *app, const QUrl &url, const QString &description)
{
	this->m_isPrefix = false;
	this->m_skipped = false;
	this->m_iconButton->hide();
	this->m_iconButton->setFile("");
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
	this->cancelPushButton->hide();
	this->pausePushButton->hide();

	this->descriptionLabel->setText(description);
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(app->resolveClipUrl(url));
	this->m_mediaPlayer->play();
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
	this->m_iconButton->show();
	this->m_iconButton->setFile(imageFile);

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
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(soundUrl);
	this->m_mediaPlayer->play();
}

void Player::skip()
{
	this->m_skipped = true;
	this->hide();
	// stop after hiding since stopping might lead to showing it again
	this->mediaPlayer()->stop();
}
