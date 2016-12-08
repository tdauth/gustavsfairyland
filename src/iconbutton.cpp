#include <QDebug>
#include <QtGui>

#include "iconbutton.h"

IconButton::IconButton(QWidget *parent) : QPushButton(parent)
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setMinimumSize(64, 64);
}

IconButton::~IconButton()
{
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
	}
	else
	{
		this->setIcon(QIcon());
	}
}

void IconButton::resizeEvent(QResizeEvent *e)
{
	QPushButton::resizeEvent(e);

	updateIcon();
}
