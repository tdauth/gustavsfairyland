#include <iostream>

#include "wondialog.h"

WonDialog::WonDialog(fairytale* app, QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
}

void WonDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			std::cerr << "Retranslate UI of won dialog" << std::endl;
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
