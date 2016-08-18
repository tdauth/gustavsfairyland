#include "clippackagedialog.h"
#include "clippackage.h"

#include "clippackagedialog.moc"

ClipPackageDialog::ClipPackageDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	this->setupUi(this);
}

void ClipPackageDialog::fill(const fairytale::ClipPackages &packages)
{
	this->m_packages = packages;

	this->comboBox->clear();

	for (fairytale::ClipPackages::const_iterator iterator = packages.begin(); iterator != packages.end(); ++iterator)
	{
		const ClipPackage *clipPackage = iterator.value();
		this->comboBox->addItem(clipPackage->name(), clipPackage->id());
	}

	if (packages.isEmpty())
	{
		this->comboBox->setEnabled(false);
		this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
	else
	{
		this->comboBox->setEnabled(true);
		this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
}

ClipPackage* ClipPackageDialog::clipPackage() const
{
	if (this->m_packages.isEmpty())
	{
		return nullptr;
	}

	return this->m_packages[this->comboBox->currentData().toString()];
}
