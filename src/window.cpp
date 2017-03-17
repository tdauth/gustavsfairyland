#include <QtGui>

#include "window.h"
#include "roomwidget.h"

Window::Window(RoomWidget *parent, Location location): QObject(parent), m_roomWidget(parent), m_location(location), m_isOpen(false)
{
}

void Window::paint(QPainter *painter, QWidget *area)
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
		const double left = 0.3;
		const double right = 0.7;

		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

		switch (m_location)
		{
			case Location::North:
			{
				x1 = left * area->size().width();
				y1 = penWidthHalf;
				x2 = right * area->size().width();
				y2 = penWidthHalf;

				break;
			}

			case Location::South:
			{
				x1 = left * area->size().width();
				y1 = area->size().height() - penWidthHalf;
				x2 = right * area->size().width();
				y2 = area->size().height() - penWidthHalf;

				break;
			}

			case Location::West:
			{
				x1 = penWidthHalf;
				y1 = left * area->size().height();
				x2 = penWidthHalf;
				y2 = right * area->size().height();

				break;
			}

			case Location::East:
			{
				x1 = area->size().width() - penWidthHalf;
				y1 = left * area->size().height();
				x2 = area->size().width() - penWidthHalf;
				y2 = right * area->size().height();

				break;
			}
		}

		painter->drawLine(x1, y1, x2, y2);
	}
	else
	{
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		int x3 = 0, y3 = 0, x4 = 0, y4 = 0;
		const double distance = 0.1;

		switch (m_location)
		{
			case Location::North:
			{
				x1 = 0.7 * area->width();
				y1 = penWidthHalf;
				x2 = 0.8 * area->width();
				y2 = distance * area->width();

				x3 = 0.3 * area->width();
				y3 = penWidthHalf;
				x4 = 0.2 * area->width();
				y4 = distance * area->width();

				break;
			}

			case Location::South:
			{
				x1 = 0.7 * area->width();
				y1 = area->height() - penWidthHalf;
				x2 = 0.8 * area->width();
				y2 = area->height() - (distance * area->height());

				x3 = 0.3 * area->width();
				y3 = area->height() - penWidthHalf;
				x4 = 0.2 * area->width();
				y4 = area->height() - (distance * area->height());

				break;
			}

			case Location::West:
			{
				x1 = penWidthHalf;
				y1 = 0.7 * area->height();
				x2 = distance * area->height();
				y2 = 0.8 * area->height();

				x3 = penWidthHalf;
				y3 = 0.3 * area->height();
				x4 = distance * area->height();
				y4 = 0.2 * area->height();

				break;
			}

			case Location::East:
			{
				x1 = area->width() - penWidthHalf;
				y1 = 0.7 * area->height();
				x2 = area->width() - (distance * area->width());
				y2 = 0.8 * area->height();

				x3 = area->width() - penWidthHalf;
				y3 = 0.3 * area->height();
				x4 = area->width() - (distance * area->width());
				y4 = 0.2 * area->height();

				break;
			}
		}

		painter->drawLine(x1, y1, x2, y2);
		painter->drawLine(x3, y3, x4, y4);
	}
}
