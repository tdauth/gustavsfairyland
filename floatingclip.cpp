#include <QtGui>

#include "floatingclip.h"
#include "clip.h"
#include "roomwidget.h"
#include "door.h"
#include "speed.h"

FloatingClip::FloatingClip(RoomWidget *parent, int width) : QObject(parent), m_roomWidget(parent), m_width(width), m_x(0), m_y(0), m_clip(nullptr), m_moveTimer(new QTimer(this))
{
	connect(this->m_moveTimer, SIGNAL(timeout()), this, SLOT(tick()));
}

void FloatingClip::paint(QPainter *painter, QWidget *area)
{
	if (m_clip != nullptr)
	{
		const int x1 = this->m_x;
		const int y1 = this->m_y;

		// draw some background if the image does not fit
		painter->fillRect(x1, y1, m_width, m_width, Qt::black);
		// scale the clip image but keep ratio
		// TODO scaling is bad for the performance?
		const int widthDifference = (m_width - m_scaledPixmap.width()) / 2;
		const int heightDifference = (m_width - m_scaledPixmap.height()) / 2;
		painter->drawPixmap(x1 + widthDifference, y1 + heightDifference, m_scaledPixmap);

		//qDebug() << "Paint clip: " << m_clip->imageUrl().toLocalFile();
	}
}

void FloatingClip::start()
{
	// set initial position to random coordinates
	this->m_x = qrand() % (this->m_roomWidget->size().height() - m_width);
	this->m_y = qrand() % (this->m_roomWidget->size().width() - m_width);

	m_moveTimer->start(FLOATING_CLIP_UPDATE_INTERVAL_MS); // tick every MS
}

void FloatingClip::pause()
{
	this->m_moveTimer->stop();
}

void FloatingClip::resume()
{
	start();
}

void FloatingClip::tick()
{
	const int distance = FLOATING_CLIP_SPEED; // distance per MS

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

	this->m_roomWidget->repaint();
}

void FloatingClip::updateScaledClipImage()
{
	this->m_scaledPixmap = QPixmap(m_clip->imageUrl().toLocalFile()).scaled(m_width, m_width, Qt::KeepAspectRatio);
}

#include "floatingclip.moc"
