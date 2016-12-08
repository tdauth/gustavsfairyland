#include "gameoverdialog.h"
#include "fairytale.h"

void GameOverDialog::retry()
{
	this->close();

	m_app->retry();
}

GameOverDialog::GameOverDialog(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app)
{
	setupUi(this);

	connect(okPushButton, &QPushButton::clicked, this, &QDialog::accept);
	//connect(retryPushButton, &QPushButton::clicked, this, &GameOverDialog::retry);
}

void GameOverDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of game over dialog";
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
