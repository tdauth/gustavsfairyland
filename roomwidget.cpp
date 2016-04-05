#include <QtGui>
#include <QMessageBox>

#include "roomwidget.h"
#include "door.h"
#include "floatingclip.h"
#include "speed.h"
#include "gamemodemoving.h"

void RoomWidget::changeWind()
{
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(0, 6); // define the range
	const int value = distr(eng);

	switch (value)
	{
		// north
		case 0:
		{
			qDebug() << "North wind";
			m_doors[(int)Door::Location::North]->open();
			m_doors[(int)Door::Location::South]->close();
			m_doors[(int)Door::Location::West]->close();
			m_doors[(int)Door::Location::East]->close();

			break;
		}

		// north west
		case 1:
		{
			qDebug() << "North west wind";
			m_doors[(int)Door::Location::North]->open();
			m_doors[(int)Door::Location::South]->close();
			m_doors[(int)Door::Location::West]->open();
			m_doors[(int)Door::Location::East]->close();

			break;
		}

		// north east
		case 2:
		{
			qDebug() << "North east wind";
			m_doors[(int)Door::Location::North]->open();
			m_doors[(int)Door::Location::South]->close();
			m_doors[(int)Door::Location::West]->close();
			m_doors[(int)Door::Location::East]->open();

			break;
		}

		// south
		case 3:
		{
			qDebug() << "South wind";
			m_doors[(int)Door::Location::North]->close();
			m_doors[(int)Door::Location::South]->open();
			m_doors[(int)Door::Location::West]->close();
			m_doors[(int)Door::Location::East]->close();

			break;
		}

		// south west
		case 4:
		{
			qDebug() << "South west wind";
			m_doors[(int)Door::Location::North]->close();
			m_doors[(int)Door::Location::South]->open();
			m_doors[(int)Door::Location::West]->open();
			m_doors[(int)Door::Location::East]->close();

			break;
		}

		// south east
		case 5:
		{
			qDebug() << "South east wind";
			m_doors[(int)Door::Location::North]->close();
			m_doors[(int)Door::Location::South]->open();
			m_doors[(int)Door::Location::West]->close();
			m_doors[(int)Door::Location::East]->open();

			break;
		}
	}
}

RoomWidget::RoomWidget(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f), m_windTimer(new QTimer(this)), m_failSound(":/fail.wav")
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	for (int i = 0; i < (int)Door::Location::MaxLocations; ++i)
	{
		m_doors.push_back(new Door(this, static_cast<Door::Location>(i)));
	}

	m_floatingClip = new FloatingClip(this);

	connect(this->m_windTimer, SIGNAL(timeout()), this, SLOT(changeWind()));
}

void RoomWidget::start()
{
	this->setEnabled(true);
	m_windTimer->start(ROOM_WIND_CHANGE_INTERVAL_MS);
	m_floatingClip->start();
}

void RoomWidget::pause()
{
	m_windTimer->stop();
	m_floatingClip->pause();
	this->setEnabled(false);
}

void RoomWidget::resume()
{
	this->setEnabled(true);
	start();
	m_floatingClip->resume();
}

void RoomWidget::paintEvent(QPaintEvent *event)
{
	//QWidget::paintEvent(event);

	QPainter painter;
	painter.begin(this);
	QBrush brush(QColor(Qt::red), Qt::SolidPattern);
	painter.setBrush(brush);
	painter.setBackground(brush);
	painter.drawRect(this->rect());

	foreach (Door *door, m_doors)
	{
		door->paint(&painter, this);
	}

	m_floatingClip->paint(&painter, this);

	painter.end();
}

void RoomWidget::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	if (this->m_floatingClip->contains(event->pos()))
	{
		emit gotIt();
	}
	else
	{
		// TODO play sound
		m_failSound.play();
	}
}


#include "roomwidget.moc"
