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
		const int width = this->size().width() * 0.9;
		const int height = this->size().height() * 0.9;
		const bool useWidth = width < height;
		const int value = useWidth ? width : height;
		this->setIconSize(QSize(value, value));

		// TODO slow
		if (useWidth)
		{
			this->setIcon(QIcon(m_pixmap.scaledToWidth(value))); // keep the ratio otherwise th image looks like shit
		}
		else
		{
			this->setIcon(QIcon(m_pixmap.scaledToHeight(value))); // keep the ratio otherwise th image looks like shit
		}
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
