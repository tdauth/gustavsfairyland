#include <QDebug>

#include "wondialog.h"

WonDialog::WonDialog(fairytale *app, QWidget *parent) : TranslatedWidget(app, parent)
{
	setupUi(this);

	connect(okPushButton, &QPushButton::clicked, this, &QDialog::accept);
}
