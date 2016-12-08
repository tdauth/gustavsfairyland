#include <QDebug>

#include "wondialog.h"

WonDialog::WonDialog(fairytale* app, QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	connect(okPushButton, &QPushButton::clicked, this, &QDialog::accept);
}

void WonDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of won dialog";
			this->retranslateUi(this);

			break;
		}

		default:
		{
			break;
		}
	}

	QDialog::changeEvent(event);
}
