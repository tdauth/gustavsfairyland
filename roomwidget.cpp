#include <QtGui>

#include "roomwidget.h"

RoomWidget::RoomWidget(QWidget *parent, Qt::WindowFlags f)
{
}

void RoomWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	QBrush brush(QColor(Qt::red));
	painter.setBackground(brush);
	painter.drawRect(this->rect());
	painter.end();
}

QPaintEngine* RoomWidget::paintEngine() const
{
	return parentWidget()->paintEngine();
}
