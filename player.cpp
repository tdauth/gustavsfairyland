/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Barade <barade.barade@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtMultimedia/QMultimedia>

#include "player.h"

Player::Player(QWidget* parent) : QDialog(parent), m_videoWidget(new QVideoWidget(this)), m_mediaPlayer(new QMediaPlayer(this)), m_skipped(false)
{
	setupUi(this);
	this->setModal(true);

	this->m_mediaPlayer->setVideoOutput(m_videoWidget);
	videoPlayerLayout->addWidget(m_videoWidget);
	m_videoWidget->show();

	volumeSlider->setValue(this->m_mediaPlayer->volume());
	connect(volumeSlider, SIGNAL(valueChanged(int)), this->m_mediaPlayer, SLOT(setVolume(int)));

	connect(this->skipPushButton, SIGNAL(clicked()), this, SLOT(skip()));
}

void Player::playVideo(const QUrl& url)
{
	this->m_skipped = false;
	this->show();
	this->skipPushButton->setEnabled(true);
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	this->m_mediaPlayer->setMedia(url);
	this->m_mediaPlayer->play();
}

void Player::skip()
{
	this->m_skipped = true;
	 this->mediaPlayer()->stop();
	 this->hide();
}
