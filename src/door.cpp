#include <iostream>

#include <QtGui>

#include "door.h"
#include "roomwidget.h"

Door::Door(RoomWidget *parent, Location location): QObject(parent), m_roomWidget(parent), m_location(location), m_isOpen(false)
{

}

void Door::paint(QPainter *painter, QWidget *area)
{
	const QColor color(Qt::black);
	QPen pen(color);
	const int penWidth = 12;
	const int penWidthHalf = 6;
	pen.setWidth(penWidth);
	pen.setStyle(Qt::SolidLine);
	painter->setPen(pen);

	// TODO use QLine and rotate them when opening the windows
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
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		int x3 = 0, y3 = 0, x4 = 0, y4 = 0;
		const int distance = 10;

		switch (m_location)
		{
			case Location::North:
			{
				x1 = 0.7 * area->width();
				y1 = penWidthHalf;
				x2 = 0.8 * area->width();
				y2 = area->width() / distance;

				x3 = 0.3 * area->width();
				y3 = penWidthHalf;
				x4 = 0.2 * area->width();
				y4 = area->width() / distance;

				break;
			}

			case Location::South:
			{
				x1 = 0.7 * area->width();
				y1 = area->height() - penWidthHalf;
				x2 = 0.8 * area->width();
				y2 = area->height() - (area->height() / distance);

				x3 = 0.3 * area->width();
				y3 = area->height() - penWidthHalf;
				x4 = 0.2 * area->width();
				y4 = area->height() - (area->height() / distance);

				break;
			}

			case Location::West:
			{
				x1 = penWidthHalf;
				y1 = 0.7 * area->height();
				x2 = area->height() / distance;
				y2 = 0.8 * area->height();

				x3 = penWidthHalf;
				y3 = 0.3 * area->height();
				x4 = area->height() / distance;
				y4 = 0.2 * area->height();

				break;
			}

			case Location::East:
			{
				x1 = area->width() - penWidthHalf;
				y1 = 0.7 * area->height();
				x2 = area->width() - (area->width() / distance);
				y2 = 0.8 * area->height();

				x3 = area->width() - penWidthHalf;
				y3 = 0.3 * area->height();
				x4 = area->width() - (area->width() / distance);
				y4 = 0.2 * area->height();

				break;
			}
		}

		painter->drawLine(x1, y1, x2, y2);
		painter->drawLine(x3, y3, x4, y4);
	}
}