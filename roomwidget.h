#ifndef ROOMWIDGET_H
#define ROOMWIDGET_H

#include <random>

#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <QtMultimedia/QSoundEffect>

class Door;
class FloatingClip;

/**
 * \brief A widget which displays a room with four windows from the top.
 *
 * Every window can open randomly and the wind can blow from that window and move the current piece of paper which contains the clip.
 * This makes it much harder for the player to select it without any hotkey.
 */
class RoomWidget : public QWidget
{
	Q_OBJECT

	signals:
		void gotIt();

	public slots:
		void changeWind();

	public:
		typedef QVector<Door*> Doors;

		RoomWidget(QWidget* parent, Qt::WindowFlags f = 0);

		void pause();
		void start();
		void resume();

		const Doors doors() const;
		FloatingClip *floatingClip() const;

	protected:
		virtual void paintEvent(QPaintEvent *event) override;
		/// Catches all clicks.
		virtual void mousePressEvent(QMouseEvent *event) override;

	private slots:
		void failSoundPlayingChanged();

	private:
		std::random_device rd; // obtain a random number from hardware
		QTimer *m_windTimer;
		Doors m_doors;
		FloatingClip *m_floatingClip;
		/// This sound effect is played whenever the player misses a click.
		QSoundEffect m_failSound;
		bool m_playNewFailSound;
		QStringList m_failSoundPaths;
};

inline const RoomWidget::Doors RoomWidget::doors() const
{
	return this->m_doors;
}

inline FloatingClip* RoomWidget::floatingClip() const
{
	return this->m_floatingClip;
}

#endif // ROOMWIDGET_H
