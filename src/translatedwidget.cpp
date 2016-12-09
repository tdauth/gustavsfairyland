#include <QtGui>

#include "translatedwidget.h"

TranslatedWidget::TranslatedWidget()
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
