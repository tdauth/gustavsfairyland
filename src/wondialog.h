#ifndef WONDIALOG_H
#define WONDIALOG_H

#include <QDialog>

#include "ui_wondialog.h"
#include "fairytale.h"
#include "translatedwidget.h"

class WonDialog : public TranslatedWidget, protected Ui::WonDialog
{
	public:
		WonDialog(fairytale *app, QWidget *parent);

		virtual void retranslateUi(QWidget *widget) override
		{
			Ui::WonDialog::retranslateUi(widget);
		}

};

#endif // WONDIALOG_H
