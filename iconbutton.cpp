#include <QtCore/QDebug>

#include "iconbutton.h"

#include "iconbutton.moc"


IconButton::IconButton(QWidget *parent) : QPushButton(parent)
{
}

void IconButton::resizeEvent(QResizeEvent* e)
{
	QWidget::resizeEvent(e);

	//qDebug() << "button was resized!";
	//this->setIconSize(QSize(this->size().width(), this->size().height()));
}
