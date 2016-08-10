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
	//qDebug() << "Repaint";
	this->repaint();
	//qDebug() << "Repaint end";
}

RoomWidget::RoomWidget(GameModeMoving *gameMode, QWidget *parent) : QWidget(parent), m_gameMode(gameMode), m_windTimer(new QTimer(this)), m_paintTimer(new QTimer(this)), m_failSound(this), m_playNewFailSound(true)
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	for (int i = 0; i < Door::MaxLocations; ++i)
	{
		m_doors.push_back(new Door(this, static_cast<Door::Location>(i)));
	}

	m_floatingClip = new FloatingClip(this);

	m_failSoundPaths.push_back("qrc:/resources/fuck1.wav");
	m_failSoundPaths.push_back("qrc:/resources/fuck2.wav");
	m_failSoundPaths.push_back("qrc:/resources/fuck3.wav");
	this->m_failSound.setSource(QUrl(m_failSoundPaths.front()));

	m_successSoundPaths.push_back("qrc:/resources/success1.wav");
	m_successSoundPaths.push_back("qrc:/resources/success2.wav");

	connect(&this->m_failSound, &QSoundEffect::playingChanged, this, &RoomWidget::failSoundPlayingChanged);

	connect(this->m_windTimer, SIGNAL(timeout()), this, SLOT(changeWind()));
	connect(this->m_paintTimer, SIGNAL(timeout()), this, SLOT(updatePaint()));
}

void RoomWidget::start()
{
	this->setEnabled(true);
	m_windTimer->start(ROOM_WIND_CHANGE_INTERVAL_MS);
	m_paintTimer->start(ROOM_REPAINT_INTERVAL_MS);
	m_floatingClip->start();
}

void RoomWidget::pause()
{
	m_windTimer->stop();
	m_paintTimer->stop();
	m_floatingClip->pause();
	this->setEnabled(false);
	this->repaint(); // repaint once disable
}

void RoomWidget::resume()
{
	this->setEnabled(true);
	start();
	m_floatingClip->resume();
}

void RoomWidget::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
	//qDebug() << "Paint event";

	QPainter painter;
	painter.begin(this);

	const QColor color = this->isEnabled() ? QColor(Qt::red) : QColor(Qt::darkRed);
	QBrush brush(color, Qt::SolidPattern);
	painter.setBrush(brush);
	painter.setBackground(brush);
	painter.drawRect(this->rect());

	foreach (Door *door, m_doors)
	{
		door->paint(&painter, this);
	}

	m_floatingClip->paint(&painter, this);

	painter.end();

	//qDebug() << "Paint event end";
}

void RoomWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	if (this->m_floatingClip->contains(event->pos()))
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

void RoomWidget::failSoundPlayingChanged()
{
	if (!m_failSound.isPlaying())
	{
		this->m_playNewFailSound = true;
		qDebug() << "Can play a new sound.";
	}
}

void RoomWidget::playSoundFromList(const QStringList &soundEffects)
{
	if (m_playNewFailSound)
	{
		m_playNewFailSound = false;
		std::mt19937 eng(rd()); // seed the generator
		std::uniform_int_distribution<> distr(0, soundEffects.size() - 1); // define the range
		const int value = distr(eng);

		m_failSound.setSource(QUrl(soundEffects[value]));
		m_failSound.play();
	}
}


#include "roomwidget.moc"
