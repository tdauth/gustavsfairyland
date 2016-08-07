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
		 // keep the ratio otherwise the image looks like shit
		const double factor = 0.98;
		const QPixmap scaled = m_pixmap.scaled(this->size().width() * factor, this->size().height() * factor, Qt::KeepAspectRatio);
		this->setIconSize(QSize(scaled.width(), scaled.height()));
		this->setIcon(QIcon(scaled));
		/*
		const int width = this->size().width();
		const int height = this->size().height();
		const bool useWidth = m_pixmap.width() > m_pixmap.height();
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
		*/
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
