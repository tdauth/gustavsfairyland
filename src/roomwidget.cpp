#include <QtGui>
#include <QMessageBox>
#include <QDesktopWidget>

#include "roomwidget.h"
#include "door.h"
#include "floatingclip.h"
#include "speed.h"
#include "gamemodemoving.h"
#include "fairytale.h"
#include "clickanimation.h"

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

	if (m_playWindSound)
	{
		m_playWindSound = false;
		m_windSoundPlayer->setMedia(QUrl("qrc:/resources/wind.wav"));
		m_windSoundPlayer->play();
	}
}

void RoomWidget::updatePaint()
{
	const qint64 interval = qMax<qint64>(m_paintTime, this->m_paintTimer->interval());

	// The elapsed time in this timer tick slot has to be measured if it is longer than the actual interval. This additional time as to be considered when updating the distance.
	m_paintTime = 0;
	QElapsedTimer overrunTimer;
	overrunTimer.start();

	//qDebug() << "Interval:" << interval;

	// move floating clips
	foreach (FloatingClip *clip, m_floatingClips)
	{
		clip->updatePosition(interval);
	}

	// update delta for click animations
	foreach (ClickAnimation *clickAnimation, m_clickAnimations)
	{
		clickAnimation->updateInterval(interval);
	}

	//qDebug() << "Repaint";

	// This should trigger an immediate paintEvent() call for RoomWidget. There have been some problems with this on Windows 7, Qt 5.7.0 mingw32 build.
	this->repaint();

	// Only indicate that it should be repainted. This does not have to trigger paintEvent() but if some paint events are skipped it doesn't matter.
	//this->update();

	//qDebug() << "Repaint end:" << overrunTimer.elapsed();

	// Now get the duration it took to repaint the whole widget which might be longer than the timer interval.
	m_paintTime = overrunTimer.elapsed();
}

void RoomWidget::windSoundStateChanged(QMediaPlayer::State state)
{
	if (state == QMediaPlayer::StoppedState)
	{
		// play loop
		if (this->isEnabled())
		{
			m_windSoundPlayer->play();
		}
		// can play again
		else
		{
			m_playWindSound = true;
		}
	}
}

int RoomWidget::floatingClipWidth() const
{
	const int availableWidth = qMax(rect().height(), rect().width());

	return availableWidth / 15;
}

int RoomWidget::floatingClipSpeed() const
{
	double factor = 0.3;

	switch (this->gameMode()->app()->difficulty())
	{
		case fairytale::Difficulty::Easy:
		{
			factor = 0.2;

			break;
		}

		case fairytale::Difficulty::Normal:
		{
			factor = 0.3;

			break;
		}

		case fairytale::Difficulty::Hard:
		{
			factor = 0.4;

			break;
		}

		// It should not be possible to win with this difficulty at all.
		case fairytale::Difficulty::Mahlerisch:
		{
			factor = 0.5;

			break;
		}
	}

	const int availableWidth = qMax(rect().height(), rect().width());
	const int result = int(double(availableWidth) * factor);

	qDebug() << "Result:" << result;

	// make sure it does not stop
	return result;
}

int RoomWidget::maxCollisionDistance() const
{
	const int availableWidth = qMax(rect().height(), rect().width());

	return availableWidth / 5;
}

RoomWidget::RoomWidget(GameModeMoving *gameMode, QWidget *parent) : RoomWidgetParent(parent), m_gameMode(gameMode), m_won(false), m_windTimer(new QTimer(this)), m_paintTimer(new QTimer(this)), m_paintTime(0), m_woodSvg(QString(":/resources/wood.svg")), m_windSoundPlayer(new QMediaPlayer(this)), m_playWindSound(true)
{
	// The room widget is painted all the time directly.
	//this->setAttribute(Qt::WA_OpaquePaintEvent);
	//this->setAttribute(Qt::WA_PaintUnclipped);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	for (int i = 0; i < Door::MaxLocations; ++i)
	{
		m_doors.push_back(new Door(this, static_cast<Door::Location>(i)));
	}

	m_floatingClips.push_back(new FloatingClip(this, this->floatingClipWidth(), this->floatingClipSpeed()));

	updateSounds();

	connect(this->m_windTimer, &QTimer::timeout, this, &RoomWidget::changeWind);
	this->m_paintTimer->setTimerType(Qt::PreciseTimer);
	connect(this->m_paintTimer, &QTimer::timeout, this, &RoomWidget::updatePaint);

	m_windSoundPlayer->setAudioRole(QAudio::GameRole);
	m_windSoundPlayer->setVolume(15);

	connect(m_windSoundPlayer, &QMediaPlayer::stateChanged, this, &RoomWidget::windSoundStateChanged);

//	qDebug() << "Current Context:" << this->format();
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

	this->setEnabled(false);

	foreach (FloatingClip *clip, m_floatingClips)
	{
		clip->pause();
	}

	foreach (Door *door, m_doors)
	{
		door->close();
	}

	this->gameMode()->app()->repaint(); // repaint the whole main window
	this->repaint(); // repaint once disable

	m_windSoundPlayer->stop();
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

void RoomWidget::addFloatingClip(const fairytale::ClipKey &clipKey, int width, int speed)
{
	FloatingClip *floatingClip = new FloatingClip(this, width, speed);
	floatingClip->setClip(clipKey);
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

void RoomWidget::clearClickAnimations()
{
	this->m_clickAnimations.clear();
}

void RoomWidget::paintEvent(QPaintEvent *event)
{
	//qDebug() << "Paint event";

	QPainter painter;
	painter.begin(this);

	//qDebug() << "Image size: " << m_woodImage.size();
	//qDebug() << "Size in paint event: " << this->size();

	if (this->isEnabled())
	{
		painter.drawImage(rect(), m_woodImage);
	}
	else
	{
		painter.drawImage(0, 0, m_woodImageDisabled);
	}

	foreach (Door *door, m_doors)
	{
		door->paint(&painter, this);
	}

	// reverse foreach, make sure the solution is always printed on top
	for (FloatingClips::reverse_iterator iterator = m_floatingClips.rbegin(); iterator != m_floatingClips.rend(); ++iterator)
	{
		(*iterator)->paint(&painter, this);
	}

	foreach (ClickAnimation *clickAnimation, this->m_clickAnimations)
	{
		clickAnimation->paint(&painter, this);
	}

	painter.end();

	//qDebug() << "Paint event end";

	QWidget::paintEvent(event);
}

void RoomWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);

	this->m_clickAnimations.push_back(new ClickAnimation(this, event->pos()));

	if (!this->m_won)
	{
		if (this->m_floatingClips.at(0)->contains(event->pos()))
		{
			playSoundFromList(m_successSoundPaths);

			this->m_won = true;

			// dont move anything anymore
			m_windTimer->stop();
			m_paintTimer->stop();
			// Pause immediately that the timer stops, otherwise there might still be time in which the player can lose. Don't pause the game mode, otherwise the release event is never triggered.
			this->gameMode()->app()->pauseTimer();
		}
		// only play the sound newly if the old is not still playing, otherwise you only here the beginning of the sound
		else
		{
			playSoundFromList(m_failSoundPaths);
		}
	}

}

void RoomWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	if (m_won)
	{
		m_won = false;

		emit gotIt();
	}
}

void RoomWidget::resizeEvent(QResizeEvent *event)
{
	// When resizeEvent() is called, the widget already has its new geometry.
	// Render SVG image whenever it is necessary
	const QSize newSize = event->size();
	m_woodImage = QImage(newSize, QImage::Format_ARGB32);
	QPainter painter(&m_woodImage);
	m_woodSvg.render(&painter);
	m_woodImageDisabled = QImage(newSize, QImage::Format_Grayscale8);
	m_woodImageDisabled.fill(Qt::transparent);
	QPainter painter2(&m_woodImageDisabled);
	m_woodSvg.render(&painter2);

	/*
	 * Whenever the room widget is resized the clips must become bigger or smaller to keep the fairness.
	 */
	foreach (FloatingClip *floatingClip, m_floatingClips)
	{
		floatingClip->setWidth(floatingClipWidth());
		floatingClip->setSpeed(floatingClipSpeed());

		// check new borders of room widget TODO maybe make floating clip X and Y relative to the room widget?
		int x = floatingClip->x();
		int y = floatingClip->y();

		if (floatingClip->x() + floatingClip->width() > newSize.width())
		{
			x = newSize.width() -  floatingClip->width();
		}

		if (floatingClip->y() + floatingClip->width() > newSize.height())
		{
			y = newSize.height() -  floatingClip->width();
		}

		floatingClip->move(x, y);
	}

	QWidget::resizeEvent(event);
}

void RoomWidget::changeEvent(QEvent* event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of about dialog";
			updateSounds();

			break;
		}

		default:
		{
			break;
		}
	}

	RoomWidgetParent::changeEvent(event);
}

void RoomWidget::playSoundFromList(const QStringList &soundEffects)
{
	if (!soundEffects.isEmpty())
	{
		std::mt19937 eng(rd()); // seed the generator
		std::uniform_int_distribution<> distr(0, soundEffects.size() - 1); // define the range
		const int value = distr(eng);
		qDebug() << "Play sound:" << soundEffects[value];
		gameMode()->app()->playSound(QUrl(soundEffects[value]));
	}
}

void RoomWidget::updateSounds()
{
	const QString language = this->m_gameMode->app()->currentTranslation();

	qDebug() << "Room widget sounds for language" << language;

	QStringList failSounds;
	failSounds.push_back(QString("qrc:/resources/fuck1_") + language + ".wav");
	failSounds.push_back(QString("qrc:/resources/fuck2_") + language + ".wav");
	failSounds.push_back(QString("qrc:/resources/fuck3_") + language + ".wav");

	m_failSoundPaths.clear();

	foreach (QString sound, failSounds)
	{
		QString fileInfoPath = sound;
		// qrc:/ is not supported by QFileInfo
		const QFileInfo fileInfo(fileInfoPath.remove(0, 3));

		if (fileInfo.exists() && fileInfo.isReadable())
		{
			m_failSoundPaths.push_back(sound);
		}
		else
		{
			qDebug() << "Invalid fail sound path" << sound;
		}
	}

	QStringList successSounds;
	successSounds.push_back(QString("qrc:/resources/success1_") + language + ".wav");
	successSounds.push_back(QString("qrc:/resources/success2_") + language + ".wav");

	m_successSoundPaths.clear();

	foreach (QString sound, successSounds)
	{
		QString fileInfoPath = sound;
		// qrc:/ is not supported by QFileInfo
		const QFileInfo fileInfo(fileInfoPath.remove(0, 3));

		if (fileInfo.exists() && fileInfo.isReadable())
		{
			m_successSoundPaths.push_back(sound);
		}
		else
		{
			qDebug() << "Invalid success sound path" << sound;
		}
	}
}

#include "roomwidget.moc"
