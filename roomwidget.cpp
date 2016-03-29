#include <QtGui>

#include "roomwidget.h"

RoomWidget::RoomWidget(QWidget *parent, Qt::WindowFlags f)
{
}

void RoomWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.end();
}

QPaintEngine* RoomWidget::paintEngine() const
{

}
