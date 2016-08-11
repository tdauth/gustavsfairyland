#include <iostream>

#include <QtGui>
#include <QtDebug>

#include "floatingclip.h"
#include "clip.h"
#include "roomwidget.h"
#include "gamemodemoving.h"
#include "fairytale.h"
#include "door.h"
#include "speed.h"

FloatingClip::FloatingClip(RoomWidget *parent, int width, int speed) : QObject(parent), m_roomWidget(parent), m_speed(speed), m_width(width), m_x(0), m_y(0), m_clip(nullptr)
{
}

void FloatingClip::paint(QPainter *painter, QWidget *area)
{
	if (m_clip != nullptr)
	{
		const int x1 = this->m_x;
		const int y1 = this->m_y;

		// draw some background if the image does not fit
		//painter->fillRect(x1, y1, m_width, m_width, Qt::black);
		// scale the clip image but keep ratio
		const int widthDifferencePaper = (m_width - m_scaledPixmapPaper.width()) / 2;
		const int heightDifferencePaper = (m_width - m_scaledPixmapPaper.height()) / 2;
		painter->drawPixmap(x1 + widthDifferencePaper, y1 + heightDifferencePaper, this->m_scaledPixmapPaper);

		// TODO scaling is bad for the performance?
		const int widthDifference = (m_width - m_scaledPixmap.width()) / 2;
		const int heightDifference = (m_width - m_scaledPixmap.height()) / 2;
		painter->drawPixmap(x1 + widthDifference, y1 + heightDifference, m_scaledPixmap);

		// paint a border to show differences between floating clips
		/*
		const QColor color = this->m_roomWidget->isEnabled() ? QColor(Qt::green) : QColor(Qt::darkGreen);
		painter->setPen(QPen(color, 12));
		painter->drawLine(this->x(), this->y(), this->x() + this->width(), this->y());
		painter->drawLine(this->x(), this->y(), this->x(), this->y() + this->width());
		painter->drawLine(this->x() + this->width(), this->y(), this->x() + this->width(), this->y() + this->width());
		painter->drawLine(this->x(), this->y() + this->width(), this->x() + this->width(), this->y() + this->width());
		*/

		//qDebug() << "Paint clip: " << m_clip->imageUrl().toLocalFile();
	}
}

void FloatingClip::start()
{
	// set initial position to random coordinates
	this->m_x = qrand() % (this->m_roomWidget->size().height() - m_width);
	this->m_y = qrand() % (this->m_roomWidget->size().width() - m_width);
}

void FloatingClip::pause()
{
}

void FloatingClip::resume()
{
	start();
}

void FloatingClip::updatePosition(int intervalMs)
{
	const int distance = (intervalMs * this->speed()) / 1000; // distance per MS
	Q_ASSERT(distance > 0);

	if (this->m_roomWidget->doors().at((int)Door::Location::North)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::South)->isOpen())
	{
		this->m_y += distance;
	}

	if (this->m_roomWidget->doors().at((int)Door::Location::South)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::North)->isOpen())
	{
		this->m_y -= distance;
	}

	if (this->m_roomWidget->doors().at((int)Door::Location::West)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::East)->isOpen())
	{
		this->m_x += distance;
	}

	if (this->m_roomWidget->doors().at((int)Door::Location::East)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::West)->isOpen())
	{
		this->m_x -= distance;
	}

	if (this->m_x + m_width > this->m_roomWidget->size().width())
	{
		this->m_x = this->m_roomWidget->size().width() - m_width;
	}
	else if (this->m_x < 0)
	{
		this->m_x = 0;
	}

	if (this->m_y + m_width > this->m_roomWidget->size().height())
	{
		this->m_y = this->m_roomWidget->size().height() - m_width;
	}
	else if (this->m_y < 0)
	{
		this->m_y = 0;
	}
}

void FloatingClip::updateScaledClipImage()
{
	m_scaledPixmapPaper = QPixmap(":/resources/paper.jpg").scaled(m_width, m_width, Qt::KeepAspectRatio);
	const int width = qMin(m_scaledPixmapPaper.width(), m_scaledPixmapPaper.height());

	const QUrl clipUrl = m_roomWidget->gameMode()->app()->resolveClipUrl(m_clip->imageUrl());
	this->m_scaledPixmap = QPixmap(clipUrl.toLocalFile()).scaled(width, width, Qt::KeepAspectRatio);
	Q_ASSERT(!this->m_scaledPixmap.isNull());
}

#include "floatingclip.moc"
