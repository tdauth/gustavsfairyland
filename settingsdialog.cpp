#include <QtWidgets>

#include "settingsdialog.h"
#include "fairytale.h"

void SettingsDialog::changeClipsDirectory()
{
	const QUrl clipsDirUrl = QFileDialog::getExistingDirectoryUrl(this, tr("Clips Directory"));

	if (!clipsDirUrl.isEmpty() && clipsDirUrl.isValid())
	{
		m_clipsDir = clipsDirUrl;
		this->clipsDirectoryLabel->setText(m_clipsDir.toString());
	}
}

void SettingsDialog::apply()
{
	this->m_app->setClipsDir(m_clipsDir);

	if (this->fullScreenCheckBox->isChecked())
	{
		this->m_app->showFullScreen();
	}
	else
	{
		this->m_app->showNormal();
	}
}

void SettingsDialog::update()
{
	this->m_clipsDir = this->m_app->clipsDir();
	this->clipsDirectoryLabel->setText(m_clipsDir.toString());
	this->fullScreenCheckBox->setChecked(this->m_app->isFullScreen());
}

SettingsDialog::SettingsDialog(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app)
{
	setupUi(this);

	connect(this->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsDialog::accept);
	connect(this->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsDialog::apply);
	connect(this->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
	connect(this->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SettingsDialog::reject);

	connect(this->clipsDirectoryPushButton, &QPushButton::clicked, this, &SettingsDialog::changeClipsDirectory);
}
