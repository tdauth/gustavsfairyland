#include "aboutdialog.h"

AboutDialog::AboutDialog(fairytale* app, QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
}
