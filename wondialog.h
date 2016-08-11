#ifndef WONDIALOG_H
#define WONDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_wondialog.h"
#include "fairytale.h"

class WonDialog : public QDialog, protected Ui::WonDialog
{
	public:
		WonDialog(fairytale *app, QWidget *parent);

	private:
		fairytale *m_app;
};

#endif // WONDIALOG_H
