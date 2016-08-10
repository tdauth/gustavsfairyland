#include <QtGui>

#include "door.h"
#include "roomwidget.h"

Door::Door(RoomWidget *parent, Location location): QObject(parent), m_roomWidget(parent), m_location(location), m_isOpen(false)
{

}

void Door::paint(QPainter *painter, QWidget *area)
{
	const QColor color = m_roomWidget->isEnabled() ? (isOpen() ? Qt::green : Qt::black) : (isOpen() ? Qt::darkGreen : Qt::black);
	QPen pen(color);
	const int penWidth = 12;
	const int penWidthHalf = 6;
	pen.setWidth(penWidth);
	painter->setPen(pen);

	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

	switch (m_location)
	{
		case Location::North:
		{
			x1 = 0.3 * area->size().width();
			y1 = penWidthHalf;
			x2 = 0.7 * area->size().width();;
			y2 = penWidthHalf;

			break;
		}

		case Location::South:
		{
			x1 = 0.3 * area->size().width();
			y1 = area->size().height() - penWidthHalf;
			x2 = 0.7 * area->size().width();;
			y2 = area->size().height() - penWidthHalf;

			break;
		}

		case Location::West:
		{
			x1 = penWidthHalf;
			y1 = 0.3 * area->size().height();
			x2 = penWidthHalf;
			y2 = 0.7 * area->size().height();

			break;
		}

		case Location::East:
		{
			x1 = area->size().width() - penWidthHalf;
			y1 = 0.3 * area->size().height();
			x2 = area->size().width() - penWidthHalf;
			y2 = 0.7 * area->size().height();

			break;
		}
	}

	painter->drawLine(x1, y1, x2, y2);
}
