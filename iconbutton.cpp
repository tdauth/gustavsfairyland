#include <QtCore/QDebug>

#include "iconbutton.h"

#include "iconbutton.moc"


IconButton::IconButton(QWidget *parent) : QPushButton(parent)
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setMinimumSize(64, 64);
}

void IconButton::updateIcon()
{
	if (!file().isEmpty())
	{
		QPixmap pixmap(m_file);
		const int min = qMin(this->size().width(), this->size().height());
		this->setIconSize(QSize(min, min));
		this->setIcon(QIcon(pixmap.scaled(min, min)));
	}
	else
	{
		this->setIcon(QIcon());
	}
}

void IconButton::resizeEvent(QResizeEvent* e)
{
	QPushButton::resizeEvent(e);

	//qDebug() << "button was resized!";
	updateIcon();
}
