#include <QtWidgets/QtWidgets>

#include "languagedialog.h"

LanguageDialog::LanguageDialog(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	this->comboBox->setItemData(0, QVariant("de"), Qt::UserRole);
	this->comboBox->setItemData(1, QVariant("en"), Qt::UserRole);

	connect(this->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
	connect(this->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::reject);
}

QString LanguageDialog::getLanguage() const
{
	return this->comboBox->currentData(Qt::UserRole).toString();
}
