#include <iostream>

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
	const int penWidth = 24;
	const int penWidthHalf = 12;
	pen.setWidth(penWidth);
	pen.setStyle(Qt::SolidLine);
	painter->setPen(pen);

	if (!this->isOpen())
	{
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

		switch (m_location)
		{
			case Location::North:
			{
				x1 = 0.3 * area->size().width();
				y1 = penWidthHalf;
				x2 = 0.7 * area->size().width();
				y2 = penWidthHalf;

				break;
			}

			case Location::South:
			{
				x1 = 0.3 * area->size().width();
				y1 = area->size().height() - penWidthHalf;
				x2 = 0.7 * area->size().width();
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
	else
	{
		QPoint p0, p1;
		QPoint p2, p3;
		const int distance = 200;

		switch (m_location)
		{
			case Location::North:
			{
				p0 = QPoint(0.7 * area->size().width(), penWidthHalf);
				p1 = QPoint(0.9 * area->size().width(), distance);

				std::cerr << "Point 0:" << p0.x() << "|" << p0.y() << std::endl;
				std::cerr << "Point 1:" << p1.x() << "|" << p1.y() << std::endl;

				p2 = QPoint(0.3 * area->size().width(), penWidthHalf);
				p3 = QPoint(0.1 * area->size().width(), distance);

				break;
			}

			case Location::South:
			{
				p0 = QPoint(0.7 * area->size().width(), area->size().height() - penWidthHalf);
				p1 = QPoint(0.9 * area->size().width(), area->size().height() - distance);

				p2 = QPoint(0.3 * area->size().width(), area->size().height() - penWidthHalf);
				p3 = QPoint(0.1 * area->size().width(), area->size().height() - distance);

				break;
			}

			case Location::West:
			{
				p0 = QPoint(penWidthHalf, 0.7 * area->size().height());
				p1 = QPoint(distance, 0.9 * area->size().height());

				p2 = QPoint(penWidthHalf, 0.3 * area->size().height());
				p3 = QPoint(distance, 0.1 * area->size().height());

				break;
			}

			case Location::East:
			{
				p0 = QPoint(area->size().width() - penWidthHalf, 0.7 * area->size().height());
				p1 = QPoint(area->size().width() - distance, 0.9 * area->size().height());

				p2 = QPoint(area->size().width() - penWidthHalf, 0.3 * area->size().height());
				p3 = QPoint(area->size().width() - distance, 0.1 * area->size().height());

				break;
			}

			painter->drawLine(p0, p1);
			painter->drawLine(p2, p3);
		}
	}
}
