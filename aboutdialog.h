#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_aboutdialog.h"
#include "fairytale.h"

class AboutDialog : public QDialog, protected Ui::AboutDialog
{
	public:
		AboutDialog(fairytale *app, QWidget *parent);

	private:
		fairytale *m_app;
};

#endif // ABOUTDIALOG_H
