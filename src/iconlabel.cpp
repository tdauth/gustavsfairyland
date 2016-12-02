#include <QDebug>

#include "iconlabel.h"

IconLabel::IconLabel(QWidget *parent) : QLabel(parent)
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setMinimumSize(64, 64);
}

IconLabel::~IconLabel()
{
}

void IconLabel::updateIcon()
{
	if (!file().isEmpty())
	{
		 // keep the ratio otherwise the image looks like shit
		const double factor = 0.98;
		const QPixmap scaled = m_pixmap.scaled(this->size().width() * factor, this->size().height() * factor, Qt::KeepAspectRatio);
		this->setPixmap(scaled);
	}
	else
	{
		this->setPixmap(QPixmap());
	}
}

void IconLabel::resizeEvent(QResizeEvent *e)
{
	QLabel::resizeEvent(e);

	//qDebug() << "button was resized!";
	updateIcon();
}
