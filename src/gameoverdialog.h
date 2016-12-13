#ifndef GAMEOVERDIALOG_H
#define GAMEOVERDIALOG_H

#include <QDialog>

#include "ui_gameoverdialog.h"
#include "fairytale.h"

class GameOverDialog : public QDialog, protected Ui::GameOverDialog
{
	Q_OBJECT

	public:
		GameOverDialog(fairytale *app, QWidget *parent);

	protected:
		virtual void changeEvent(QEvent *event) override;

	private:
		fairytale *m_app;
};

#endif // GAMEOVERDIALOG_H
