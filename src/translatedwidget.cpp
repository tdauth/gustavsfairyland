#include <QtGui>

#include "translatedwidget.h"
#include "fairytale.h"

TranslatedWidget::TranslatedWidget(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app)
{
}

void TranslatedWidget::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of about dialog";
			this->retranslateUi(this);

			break;
		}

		default:
		{
			break;
		}
	}

	QWidget::changeEvent(event);
}

void TranslatedWidget::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	app()->updateSize(this);
}
