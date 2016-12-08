#ifndef CLICKANIMATION_H
#define CLICKANIMATION_H

#include <QObject>
#include <QPainter>
#include <QWidget>

class RoomWidget;

/**
 * \brief A simple animation in form of a circle to show the player where he or she has clicked.
 * The radius becomes bigger the longer the animation is shown until it disappears.
 */
class ClickAnimation : public QObject
{
	public:
		ClickAnimation(RoomWidget *parent, const QPoint &position);

		/**
		 * Paints the click animation with \p painter on \p area.
		 */
		void paint(QPainter *painter, QWidget *area);

		void updateInterval(qint64 interval);

		bool isDone() const;

		/**
		 * \return Returns the current radius of the animation which depends on the elapsed time and size of the room widget.
		 */
		int radius() const;

	private:
		RoomWidget *m_roomWidget;
		QPoint m_position;
		/*
		 * Total elapsed interval in MS.
		 */
		qint64 m_totalInterval;
};

inline void ClickAnimation::updateInterval(qint64 interval)
{
	this->m_totalInterval += interval;
}

inline bool ClickAnimation::isDone() const
{
	return this->m_totalInterval > 500;
}

#endif // CLICKANIMATION_H
