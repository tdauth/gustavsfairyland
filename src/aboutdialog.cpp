#include <iostream>

#include "aboutdialog.h"

AboutDialog::AboutDialog(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app)
{
	setupUi(this);

	connect(okPushButton, &QPushButton::clicked, this, &QDialog::accept);
}

void AboutDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			this->retranslateUi(this);
			// Try to load the current locale. If no translation file exists it will remain English.
			QString locale = QLocale::system().name();
			locale.truncate(locale.lastIndexOf('_'));
			qDebug() << "Retranslate UI of about dialog with locale" << locale;

			const QFileInfo fileInfo(":/resources/splash" + locale + ".jpg");

			if (fileInfo.exists())
			{
				label->setPixmap(QPixmap(fileInfo.absoluteFilePath()));
			}
			else
			{
				qDebug() << fileInfo.absoluteFilePath() << "does not exist!";
			}

			break;
		}

		default:
		{
			break;
		}
	}

	QDialog::changeEvent(event);
}