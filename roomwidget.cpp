#include <QtGui>
#include <QMessageBox>

#include "roomwidget.h"
#include "door.h"
#include "floatingclip.h"
#include "speed.h"
#include "gamemodemoving.h"
#include "fairytale.h"

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
			//qDebug() << "North wind";
			m_doors[(int)Door::Location::North]->open();
			m_doors[(int)Door::Location::South]->close();
			m_doors[(int)Door::Location::West]->close();
			m_doors[(int)Door::Location::East]->close();

			break;
		}

		// north west
		case 1:
		{
			//qDebug() << "North west wind";
			m_doors[(int)Door::Location::North]->open();
			m_doors[(int)Door::Location::South]->close();
			m_doors[(int)Door::Location::West]->open();
			m_doors[(int)Door::Location::East]->close();

			break;
		}

		// north east
		case 2:
		{
			//qDebug() << "North east wind";
			m_doors[(int)Door::Location::North]->open();
			m_doors[(int)Door::Location::South]->close();
			m_doors[(int)Door::Location::West]->close();
			m_doors[(int)Door::Location::East]->open();

			break;
		}

		// south
		case 3:
		{
			//qDebug() << "South wind";
			m_doors[(int)Door::Location::North]->close();
			m_doors[(int)Door::Location::South]->open();
			m_doors[(int)Door::Location::West]->close();
			m_doors[(int)Door::Location::East]->close();

			break;
		}

		// south west
		case 4:
		{
			//qDebug() << "South west wind";
			m_doors[(int)Door::Location::North]->close();
			m_doors[(int)Door::Location::South]->open();
			m_doors[(int)Door::Location::West]->open();
			m_doors[(int)Door::Location::East]->close();

			break;
		}

		// south east
		case 5:
		{
			//qDebug() << "South east wind";
			m_doors[(int)Door::Location::North]->close();
			m_doors[(int)Door::Location::South]->open();
			m_doors[(int)Door::Location::West]->close();
			m_doors[(int)Door::Location::East]->open();

			break;
		}
	}
}

void RoomWidget::updatePaint()
{
	// move floating clips
	foreach (FloatingClip *clip, m_floatingClips)
	{
		clip->updatePosition(this->m_paintTimer->interval());
	}

	//qDebug() << "Repaint";
	this->repaint();
	//qDebug() << "Repaint end";
}

RoomWidget::RoomWidget(GameModeMoving *gameMode, QWidget *parent) : QWidget(parent), m_gameMode(gameMode), m_windTimer(new QTimer(this)), m_paintTimer(new QTimer(this)), m_woodSvg(QString(":/resources/wood.svg"))
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	for (int i = 0; i < Door::MaxLocations; ++i)
	{
		m_doors.push_back(new Door(this, static_cast<Door::Location>(i)));
	}

	m_floatingClips.push_back(new FloatingClip(this));

	m_failSoundPaths.push_back("qrc:/resources/fuck1.wav");
	m_failSoundPaths.push_back("qrc:/resources/fuck2.wav");
	m_failSoundPaths.push_back("qrc:/resources/fuck3.wav");

	m_successSoundPaths.push_back("qrc:/resources/success1.wav");
	m_successSoundPaths.push_back("qrc:/resources/success2.wav");

	connect(this->m_windTimer, SIGNAL(timeout()), this, SLOT(changeWind()));
	connect(this->m_paintTimer, SIGNAL(timeout()), this, SLOT(updatePaint()));
}

void RoomWidget::start()
{
	this->setEnabled(true);
	m_windTimer->start(ROOM_WIND_CHANGE_INTERVAL_MS);
	m_paintTimer->start(ROOM_REPAINT_INTERVAL_MS);

	foreach (FloatingClip *clip, m_floatingClips)
	{
		clip->start();
	}
}

void RoomWidget::pause()
{
	m_windTimer->stop();
	m_paintTimer->stop();

	foreach (FloatingClip *clip, m_floatingClips)
	{
		clip->pause();
	}

	this->setEnabled(false);
	this->repaint(); // repaint once disable
}

void RoomWidget::resume()
{
	this->setEnabled(true);
	start();

	foreach (FloatingClip *clip, m_floatingClips)
	{
		clip->resume();
	}
}

void RoomWidget::addFloatingClip(Clip *clip, int width, int speed)
{
	FloatingClip *floatingClip = new FloatingClip(this, width, speed);
	floatingClip->setClip(clip);
	m_floatingClips.push_back(floatingClip);
}

void RoomWidget::clearFloatingClipsExceptFirst()
{
	for (int i = 1; i < this->m_floatingClips.size(); ++i)
	{
		delete this->m_floatingClips.at(i);
	}

	this->m_floatingClips.resize(1);
}

void RoomWidget::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
	//qDebug() << "Paint event";

	QPainter painter;
	painter.begin(this);

	/*
	const QColor color = this->isEnabled() ? QColor(Qt::red) : QColor(Qt::darkRed);
	QBrush brush(color, Qt::SolidPattern);
	painter.setBrush(brush);
	painter.setBackground(brush);
	painter.drawRect(this->rect());
	*/
	// TODO slow
	painter.drawImage(0, 0, m_woodImage);

	foreach (Door *door, m_doors)
	{
		door->paint(&painter, this);
	}

	// reverse foreach, make sure the solution is always printed on top
	for (FloatingClips::reverse_iterator iterator = m_floatingClips.rbegin(); iterator != m_floatingClips.rend(); ++iterator)
	{
		(*iterator)->paint(&painter, this);
	}

	painter.end();

	//qDebug() << "Paint event end";
}

void RoomWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	if (this->m_floatingClips.at(0)->contains(event->pos()))
	{
		playSoundFromList(m_successSoundPaths);

		emit gotIt();
	}
	// only play the sound newly if the old is not still playing, otherwise you only here the beginning of the sound
	else
	{
		playSoundFromList(m_failSoundPaths);
	}
}

void RoomWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	qDebug() << "Resize SVG";
	// Render SVG image whenever it is necessary
	// TODO slow?
	m_woodImage = QImage(this->rect().width(), this->rect().height(), QImage::Format_ARGB32);
	QPainter painter(&m_woodImage);
	m_woodSvg.render(&painter);
}

void RoomWidget::playSoundFromList(const QStringList &soundEffects)
{
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(0, soundEffects.size() - 1); // define the range
	const int value = distr(eng);
	qDebug() << "Play sound:" << soundEffects[value];
	gameMode()->app()->playSound(QUrl(soundEffects[value]));
}


#include "roomwidget.moc"
