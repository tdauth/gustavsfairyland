#include <QtGui>

#include "clickanimation.h"
#include "roomwidget.h"

ClickAnimation::ClickAnimation(RoomWidget *parent, const QPoint &position) : QObject(parent), m_roomWidget(parent), m_position(position), m_totalInterval(0)
{

}


void ClickAnimation::paint(QPainter *painter, QWidget *area)
{
	if (isDone())
	{
		return;
	}

	const QColor color(this->m_roomWidget->isEnabled() ? Qt::black : Qt::black);
	QPen pen(color);
	const int penWidth = 4;
	pen.setWidth(penWidth);
	pen.setStyle(Qt::SolidLine);
	painter->setPen(pen);

	const int radius = this->m_totalInterval / 8;

	painter->drawPoint(m_position);
	painter->drawEllipse(m_position, radius, radius);
}
