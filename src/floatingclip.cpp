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

FloatingClip::FloatingClip(RoomWidget *parent, int width, int speed) : QObject(parent), m_roomWidget(parent), m_speed(speed), m_width(width), m_x(0), m_y(0), m_dirX(1), m_dirY(1), m_collisionDistance(0), m_clip(nullptr)
{
}

void FloatingClip::paint(QPainter *painter, QWidget *area)
{
	if (m_clip != nullptr)
	{
		const int x1 = this->x();
		const int y1 = this->y();

		const QImage &imagePaper = this->m_roomWidget->isEnabled() ? m_scaledImagePaper : m_scaledImagePaperDisabled;
		const QRect paperRect(x1, y1, imagePaper.width(), imagePaper.height());
		painter->drawImage(paperRect, imagePaper);

		// paint the image in the center of the paper
		const QImage &image = this->m_roomWidget->isEnabled() ? m_scaledImage : m_scaledImageDisabled;
		const int heightDifference = (imagePaper.height() - image.height()) / 2;
		const int widthDifference = (imagePaper.width() - image.width()) / 2;
		painter->drawImage(x1 + widthDifference, y1 + heightDifference, image);
	}
}

void FloatingClip::setWidth(int width)
{
	this->m_width = width;
	updateScaledClipImage();
}

void FloatingClip::start()
{
	this->updateScaledClipImage();
	// set initial position to random coordinates
	this->m_x = qrand() % (this->m_roomWidget->size().height() - m_scaledImagePaper.height());
	this->m_y = qrand() % (this->m_roomWidget->size().width() - m_scaledImagePaper.width());

	// set random start dir
	const int randomDirX = qrand() % 1;

	if (randomDirX == 0)
	{
		this->m_dirX = 1;
	}
	else
	{
		this->m_dirX = -1;
	}

	const int randomDirY = qrand() % 1;

	if (randomDirY == 0)
	{
		this->m_dirY = 1;
	}
	else
	{
		this->m_dirY = -1;
	}
}

void FloatingClip::pause()
{
}

void FloatingClip::resume()
{
	// reset position, otherwise it would be unfair to pause and then just know the clip's position
	start();
}

void FloatingClip::updatePosition(qint64 elapsedTime)
{
	if (elapsedTime <= 0)
	{
		return;
	}

	// make sure the clip does not stop even if it leads to fast clips when the screen is too small
	const int distance = qMax<qint64>(1, elapsedTime * this->speed() / 1000); // distance per S

	// Only update the position if the elapsed time is long enough, otherwise the clip won't move for this time.
	if (distance > 0)
	{
		int x = this->x();
		int y = this->y();
		const int height = this->m_scaledImagePaper.height();
		const int width = this->m_scaledImagePaper.width();
		bool collided = false;

		// Check for collisions with other clips.
		foreach (FloatingClip *otherClip, this->m_roomWidget->floatingClips())
		{
			if (otherClip != this)
			{
				const QRect clipRect(this->x(), this->y(), width, height);
				const QRect otherClipRect(otherClip->x(), otherClip->y(), otherClip->m_scaledImagePaper.width(), otherClip->m_scaledImagePaper.height());

				if (clipRect.intersects(otherClipRect))
				{
					if (otherClip->x() > this->x())
					{
						setDirX(-1);
					}
					else
					{
						setDirX(1);
					}

					if (otherClip->y() > this->y())
					{
						setDirY(-1);
					}
					else
					{
						setDirY(1);
					}

					collided = true;

					break;
				}
			}
		}

		const int roomWidth = this->m_roomWidget->size().width();
		const int roomHeight = this->m_roomWidget->size().height();

		// Check for collisions with the boundary.
		if (this->x() + width == roomWidth)
		{
			setDirX(-1);
			collided = true;
		}
		else if (this->x() == 0)
		{
			setDirX(1);
			collided = true;
		}

		if (this->y() + height == roomHeight)
		{
			setDirY(-1);
			collided = true;
		}
		else if (this->y() == 0)
		{
			setDirY(1);
			collided = true;
		}

		// check the wind first and calculate direction
		if (!collided && m_collisionDistance == 0)
		{
			if (this->m_roomWidget->doors().at((int)Door::Location::North)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::South)->isOpen())
			{
				setDirY(1);
			}

			if (this->m_roomWidget->doors().at((int)Door::Location::South)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::North)->isOpen())
			{
				setDirY(-1);
			}

			if (this->m_roomWidget->doors().at((int)Door::Location::West)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::East)->isOpen())
			{
				setDirX(1);
			}

			if (this->m_roomWidget->doors().at((int)Door::Location::East)->isOpen() && !this->m_roomWidget->doors().at((int)Door::Location::West)->isOpen())
			{
				setDirX(-1);
			}
		}
		// restart collision counter
		else if (collided)
		{
			m_collisionDistance = distance;
		}
		// move from collision a bit more
		else
		{
			m_collisionDistance += distance;

			// reset collision movement, wind gets priority
			if (m_collisionDistance > this->m_roomWidget->maxCollisionDistance())
			{
				m_collisionDistance = 0;
			}
		}

		// Move the floating clip in the specified direction.
		x += this->dirX() * distance;
		y += this->dirY() * distance;

		// Make sure boundaries of the room are not violated.
		if (x + width > roomWidth)
		{
			x = roomWidth - width;
		}
		else if (x < 0)
		{
			x = 0;
		}

		if (y + height > roomHeight)
		{
			y = roomHeight - height;
		}
		else if (y < 0)
		{
			y = 0;
		}

		this->move(x, y);
	}
}

void FloatingClip::updateScaledClipImage()
{
	m_scaledImagePaper = QImage(":/resources/paper.jpg").scaled(m_width, m_width, Qt::KeepAspectRatio);
	const int width = qMin(m_scaledImagePaper.width(), m_scaledImagePaper.height());

	m_scaledImagePaperDisabled = QImage(":/resources/paper.jpg").convertToFormat(QImage::Format_Grayscale8).scaled(m_width, m_width, Qt::KeepAspectRatio);

	if (m_clip != nullptr)
	{
		const QUrl clipUrl = m_roomWidget->gameMode()->app()->resolveClipUrl(m_clip->imageUrl());
#ifndef Q_OS_ANDROID
		const QString filePath = clipUrl.toLocalFile();
#else
		const QString filePath = clipUrl.url();
#endif
		this->m_scaledImage = QImage(filePath).scaled(width, width, Qt::KeepAspectRatio);
		Q_ASSERT(!this->m_scaledImage.isNull());
		this->m_scaledImageDisabled = QImage(filePath).convertToFormat(QImage::Format_Grayscale8).scaled(width, width, Qt::KeepAspectRatio);
	}
}

#include "floatingclip.moc"