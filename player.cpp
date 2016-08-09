#include <QtMultimedia/QMultimedia>
#include <QMessageBox>

#include "player.h"
#include "fairytale.h"

Player::Player(QWidget* parent, fairytale *app) : QDialog(parent), m_app(app), m_videoWidget(new QVideoWidget(this)), m_mediaPlayer(new QMediaPlayer(this)), m_skipped(false)
{
	setupUi(this);
	this->setModal(true);

	this->m_mediaPlayer->setVideoOutput(m_videoWidget);

	// expanding makes sure that it uses the maximum possible size
	this->m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->m_videoWidget->setMinimumSize(QSize(240, 240));
	videoPlayerLayout->addWidget(m_videoWidget);
	m_videoWidget->show();

	volumeSlider->setValue(this->m_mediaPlayer->volume());
	connect(volumeSlider, SIGNAL(valueChanged(int)), this->m_mediaPlayer, SLOT(setVolume(int)));
	this->m_mediaPlayer->setVolume(volumeSlider->value());

	connect(this->skipPushButton, SIGNAL(clicked()), this, SLOT(skip()));
	connect(this->pausePushButton, SIGNAL(clicked()), app, SLOT(pauseGame()));
	connect(this->cancelPushButton, &QPushButton::clicked, app, &fairytale::cancelGame);
	connect(this, SIGNAL(rejected()), this, SLOT(skip()));
}

void Player::playVideo(fairytale *app, const QUrl& url, const QString &description)
{
	this->m_skipped = false;
	this->show();
	this->skipPushButton->setEnabled(true);

	this->descriptionLabel->setText(description);
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(app->resolveClipUrl(url));
	this->m_mediaPlayer->play();
}

void Player::skip()
{
	this->m_skipped = true;
	this->hide();
	// stop after hiding since stopping might lead to showing it again
	this->mediaPlayer()->stop();
}
