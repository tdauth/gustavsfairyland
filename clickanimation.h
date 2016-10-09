#ifndef CLICKANIMATION_H
#define CLICKANIMATION_H

#include <QObject>
#include <QPainter>
#include <QWidget>

class RoomWidget;

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

	private:
		RoomWidget *m_roomWidget;
		QPoint m_position;
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
