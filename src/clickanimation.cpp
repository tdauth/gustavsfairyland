#include <QtGui>

#include "clickanimation.h"
#include "roomwidget.h"

ClickAnimation::ClickAnimation(RoomWidget *parent, const QPoint &position) : QObject(parent), m_roomWidget(parent), m_position(position), m_totalInterval(0)
{

}

int ClickAnimation::radius() const
{
	const int availableWidth = qMax(m_roomWidget->rect().height(), m_roomWidget->rect().width());

	return this->m_totalInterval * availableWidth / 10000;
}

void ClickAnimation::paint(QPainter *painter, QWidget *area)
{
	if (isDone())
	{
		return;
	}

	const QColor color(Qt::black);
	QPen pen(color);
	const int penWidth = 4;
	pen.setWidth(penWidth);
	pen.setStyle(Qt::SolidLine);
	painter->setPen(pen);

	const int radius = this->radius();

	painter->drawPoint(m_position);
	painter->drawEllipse(m_position, radius, radius);
}
