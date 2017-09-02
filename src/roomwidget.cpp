#include <QtGui>
#include <QMessageBox>
#include <QDesktopWidget>

#include "roomwidget.h"
#include "window.h"
#include "floatingclip.h"
#include "speed.h"
#include "gamemodemoving.h"
#include "fairytale.h"
#include "clickanimation.h"
#include "solutionwidget.h"
#include "utilities.h"

void RoomWidget::changeWind()
{
	const int value = randomNumber(0, 6);

	switch (value)
	{
		// north
		case 0:
		{
			//qDebug() << "North wind";
			m_windows[(int)Window::Location::North]->open();
			m_windows[(int)Window::Location::South]->close();
			m_windows[(int)Window::Location::West]->close();
			m_windows[(int)Window::Location::East]->close();

			break;
		}

		// north west
		case 1:
		{
			//qDebug() << "North west wind";
			m_windows[(int)Window::Location::North]->open();
			m_windows[(int)Window::Location::South]->close();
			m_windows[(int)Window::Location::West]->open();
			m_windows[(int)Window::Location::East]->close();

			break;
		}

		// north east
		case 2:
		{
			//qDebug() << "North east wind";
			m_windows[(int)Window::Location::North]->open();
			m_windows[(int)Window::Location::South]->close();
			m_windows[(int)Window::Location::West]->close();
			m_windows[(int)Window::Location::East]->open();

			break;
		}

		// south
		case 3:
		{
			//qDebug() << "South wind";
			m_windows[(int)Window::Location::North]->close();
			m_windows[(int)Window::Location::South]->open();
			m_windows[(int)Window::Location::West]->close();
			m_windows[(int)Window::Location::East]->close();

			break;
		}

		// south west
		case 4:
		{
			//qDebug() << "South west wind";
			m_windows[(int)Window::Location::North]->close();
			m_windows[(int)Window::Location::South]->open();
			m_windows[(int)Window::Location::West]->open();
			m_windows[(int)Window::Location::East]->close();

			break;
		}

		// south east
		case 5:
		{
			//qDebug() << "South east wind";
			m_windows[(int)Window::Location::North]->close();
			m_windows[(int)Window::Location::South]->open();
			m_windows[(int)Window::Location::West]->close();
			m_windows[(int)Window::Location::East]->open();

			break;
		}
	}

	if (m_playWindSound)
	{
		m_playWindSound = false;
		m_windSoundPlayer->setMedia(QUrl("qrc:/resources/wind.wav"));
		m_windSoundPlayer->setVolume(this->gameMode()->app()->musicVolume());
		m_windSoundPlayer->setMuted(this->gameMode()->app()->isMusicMuted());
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

	// This should trigger an immediate paintEvent() call for RoomWidget. There have been some problems with this on Windows 7, Qt 5.7.0 mingw32 build.
	this->repaint();

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
			qDebug() << "Replay wind sound since room widget is enabled";
			m_windSoundPlayer->setVolume(this->gameMode()->app()->musicVolume());
			m_windSoundPlayer->setMuted(this->gameMode()->app()->isMusicMuted());
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
	const long long availableWidth = ((long long)(rect().height()) + (long long)(rect().width())) / 2;

	return qMax<long long>(1, (long long)((long double)availableWidth * (long double)0.1));
}

int RoomWidget::floatingClipSpeed() const
{
	long double factor = 0.3;

	if (!fairytale::hasTouchDevice())
	{
		qDebug() << "Has no touch device!";

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
	}
	else
	{
		qDebug() << "Has touch device!";

		switch (this->gameMode()->app()->difficulty())
		{
			case fairytale::Difficulty::Easy:
			{
				factor = 0.6;

				break;
			}

			case fairytale::Difficulty::Normal:
			{
				factor = 0.7;

				break;
			}

			case fairytale::Difficulty::Hard:
			{
				factor = 0.8;

				break;
			}

			// It should not be possible to win with this difficulty at all.
			case fairytale::Difficulty::Mahlerisch:
			{
				factor = 0.9;

				break;
			}
		}
	}

	const long long availableWidth = ((long long)(rect().height()) + (long long)(rect().width())) / 2;
	const int result = (long double)(availableWidth) * factor * floatingClipSpeedFactor();

	qDebug() << "Result:" << result;

	// make sure it does not stop
	return qMax(1, result);
}

void RoomWidget::dragEnterEvent(QDragEnterEvent *event)
{
	FloatingClip *floatingClip = dynamic_cast<FloatingClip*>(event->source());

	if (floatingClip != nullptr)
	{
		qDebug() << "Accept drop in room widget!";
		event->acceptProposedAction();
	}
}


void RoomWidget::dropEvent(QDropEvent *event)
{
	qDebug() << "Drop event in room widget!";
	FloatingClip *floatingClip = dynamic_cast<FloatingClip*>(event->source());

	if (floatingClip != nullptr)
	{
		// accept but reset it in the game mode later
		event->acceptProposedAction();

		if (m_solutionWidget != nullptr)
		{
			m_solutionWidget->fail(floatingClip->clipKey());
		}
	}
	else
	{
		qDebug() << "Unable to find corresponding floating clip.";
	}

	QWidget::dropEvent(event);
}

int RoomWidget::maxCollisionDistance() const
{
	const int availableWidth = qMax(rect().height(), rect().width());

	return availableWidth / 5;
}

RoomWidget::RoomWidget(GameMode *gameMode, Mode mode, QWidget *parent) : RoomWidgetParent(parent), m_gameMode(gameMode), m_mode(mode), m_won(false), m_windTimer(new QTimer(this)), m_paintTimer(new QTimer(this)), m_paintTime(0), m_woodSvg(QString(":/resources/wood.svg")), m_windSoundPlayer(new QMediaPlayer(this)), m_playWindSound(true), m_floatingClipSpeedFactor(1.0), m_solutionWidget(nullptr)
{
	// The room widget is painted all the time directly.
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	for (int i = 0; i < Window::MaxLocations; ++i)
	{
		m_windows.push_back(new Window(this, static_cast<Window::Location>(i)));
	}

	updateSounds();

	connect(this->m_windTimer, &QTimer::timeout, this, &RoomWidget::changeWind);
	this->m_paintTimer->setTimerType(Qt::PreciseTimer);
	connect(this->m_paintTimer, &QTimer::timeout, this, &RoomWidget::updatePaint);

	m_windSoundPlayer->setAudioRole(QAudio::GameRole);
	m_windSoundPlayer->setVolume(fairytale::defaultMusicVolume);

	connect(m_windSoundPlayer, &QMediaPlayer::stateChanged, this, &RoomWidget::windSoundStateChanged);

	if (mode == Mode::DragAndDrop)
	{
		this->setAcceptDrops(true);
	}
}

RoomWidget::~RoomWidget()
{
	clearFloatingClips();
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

	for (FloatingClip *clip : m_floatingClips)
	{
		clip->pause();
	}

	for (Window *window : m_windows)
	{
		window->close();
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

	m_floatingClips.insert(clipKey, floatingClip);
}

void RoomWidget::clearFloatingClips()
{
	for (FloatingClip *floatingClip : m_floatingClips)
	{
		delete floatingClip;
	}

	this->m_floatingClips.clear();

	for (FloatingClip *floatingClip : m_hiddenFloatingClips)
	{
		delete floatingClip;
	}

	this->m_hiddenFloatingClips.clear();
}

void RoomWidget::removeFloatingClip(const fairytale::ClipKey &clipKey)
{
	FloatingClips::iterator iterator = this->m_floatingClips.find(clipKey);

	if (iterator != this->m_floatingClips.end())
	{
		FloatingClip *floatingClip = iterator.value();
		this->m_floatingClips.erase(iterator);
		delete floatingClip;
		floatingClip = nullptr;
	}
}

void RoomWidget::hideFloatingClip(const fairytale::ClipKey &clipKey)
{
	FloatingClips::iterator iterator = this->m_floatingClips.find(clipKey);

	if (iterator != this->m_floatingClips.end())
	{
		FloatingClip *floatingClip = iterator.value();
		this->m_floatingClips.erase(iterator);
		this->m_hiddenFloatingClips.insert(clipKey, floatingClip);
	}
}

void RoomWidget::clearClickAnimations()
{
	this->m_clickAnimations.clear();
}

void RoomWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);

	if (this->isEnabled())
	{
		painter.drawImage(rect(), m_woodImage);
	}
	else
	{
		painter.drawImage(0, 0, m_woodImageDisabled);
	}

	// TODO reverse foreach, make sure the solution is always printed on top
	for (FloatingClips::iterator iterator = m_floatingClips.begin(); iterator != m_floatingClips.end(); ++iterator)
	{
		(*iterator)->paint(&painter);
	}

	// paint windows over the floating clips
	for (Window *window : m_windows)
	{
		window->paint(&painter, this);
	}

	for (ClickAnimation *clickAnimation : this->m_clickAnimations)
	{
		clickAnimation->paint(&painter, this);
	}

	painter.end();

	QWidget::paintEvent(event);
}

void RoomWidget::playSuccessSound()
{
	playSoundFromList(m_successSoundPaths, true);
}

void RoomWidget::playFailSound()
{
	playSoundFromList(m_failSoundPaths, false);
}

void RoomWidget::cancelDrags()
{
	for (FloatingClip *floatingClip : m_floatingClips)
	{
		// Stop Drag & Drop on pausing.
		QMouseEvent *finishMoveEvent = new QMouseEvent(QEvent::MouseButtonRelease, floatingClip->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
		qApp->sendEvent(floatingClip, finishMoveEvent);
	}
}

void RoomWidget::mouseClick(const QPoint &pos)
{
	this->m_clickAnimations.push_back(new ClickAnimation(this, pos));

	if (!this->m_won)
	{
		if (this->m_floatingClips[this->gameMode()->solutions().front()]->contains(pos))
		{
			playSuccessSound();

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
			playFailSound();
		}
	}
}

void RoomWidget::mouseRelease()
{
	if (m_won)
	{
		m_won = false;

		/*
		 * Make sure that the success sound is not cut.
		 */
		this->waitForSuccessSound();

		emit gotIt();
	}
}

void RoomWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);

	if (mode() == Mode::Click)
	{
		this->mouseClick(event->pos());
	}
}

void RoomWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	if (mode() == Mode::Click)
	{
		this->mouseRelease();
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

void RoomWidget::playSoundFromList(const QStringList &soundEffects, bool immediately)
{
	if (!soundEffects.isEmpty())
	{
		const int value = randomNumber(0, soundEffects.size() - 1);
		qDebug() << "Play sound:" << soundEffects[value];
		gameMode()->app()->playSound(QUrl(soundEffects[value]), immediately);
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

void RoomWidget::waitForSuccessSound()
{
	QEventLoop eventLoop(this);

	while (gameMode()->app()->isSoundPlaying())
	{
		eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents, 500);
	}
}

#include "roomwidget.moc"
