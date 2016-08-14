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

		const QImage &imagePaper = this->m_roomWidget->isEnabled() ? m_scaledImagePaper : m_scaledImagePaperDisabled;
		const QImage &image = this->m_roomWidget->isEnabled() ? m_scaledImage : m_scaledImageDisabled;

		// draw some background if the image does not fit
		//painter->fillRect(x1, y1, m_width, m_width, Qt::black);
		// scale the clip image but keep ratio
		const int widthDifferencePaper = (m_width - imagePaper.width()) / 2;
		const int heightDifferencePaper = (m_width - imagePaper.height()) / 2;
		painter->drawImage(x1 + widthDifferencePaper, y1 + heightDifferencePaper, imagePaper);

		// TODO scaling is bad for the performance?
		const int widthDifference = (m_width - image.width()) / 2;
		const int heightDifference = (m_width - image.height()) / 2;
		painter->drawImage(x1 + widthDifference, y1 + heightDifference, image);

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

QPair<int,int> FloatingClip::updatePosition(qint64 elapsedTime)
{
	int x = this->x();
	int y = this->y();
	QPair<int, int> result(x, y);
	const int distance = (elapsedTime * this->speed()) / 1000; // distance per MS

	if (distance > 0)
	{
		if (this->m_roomWidget->doors().at((int)Door::Location::North)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::South)->isOpen())
		{
			y += distance;
		}

		if (this->m_roomWidget->doors().at((int)Door::Location::South)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::North)->isOpen())
		{
			y -= distance;
		}

		if (this->m_roomWidget->doors().at((int)Door::Location::West)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::East)->isOpen())
		{
			x += distance;
		}

		if (this->m_roomWidget->doors().at((int)Door::Location::East)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::West)->isOpen())
		{
			x -= distance;
		}

		if (x + m_width > this->m_roomWidget->size().width())
		{
			x = this->m_roomWidget->size().width() - m_width;
		}
		else if (x < 0)
		{
			x = 0;
		}

		if (y + m_width > this->m_roomWidget->size().height())
		{
			y = this->m_roomWidget->size().height() - m_width;
		}
		else if (y < 0)
		{
			y = 0;
		}

		result.first = x;
		result.second = y;
	}

	return result;
}

void FloatingClip::updateScaledClipImage()
{
	m_scaledImagePaper = QImage(":/resources/paper.jpg").scaled(m_width, m_width, Qt::KeepAspectRatio);
	const int width = qMin(m_scaledImagePaper.width(), m_scaledImagePaper.height());

	m_scaledImagePaperDisabled = QImage(":/resources/paper.jpg").convertToFormat(QImage::Format_Grayscale8).scaled(m_width, m_width, Qt::KeepAspectRatio);

	const QUrl clipUrl = m_roomWidget->gameMode()->app()->resolveClipUrl(m_clip->imageUrl());
	this->m_scaledImage = QImage(clipUrl.toLocalFile()).scaled(width, width, Qt::KeepAspectRatio);
	Q_ASSERT(!this->m_scaledImage.isNull());
	this->m_scaledImageDisabled = QImage(clipUrl.toLocalFile()).convertToFormat(QImage::Format_Grayscale8).scaled(width, width, Qt::KeepAspectRatio);
}

#include "floatingclip.moc"
