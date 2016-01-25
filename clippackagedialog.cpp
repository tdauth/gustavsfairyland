#include "clippackagedialog.h"
#include "clippackage.h"

#include "clippackagedialog.moc"

ClipPackageDialog::ClipPackageDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	this->setupUi(this);
}

void ClipPackageDialog::fill(const fairytale::ClipPackages& packages)
{
	this->m_packages = packages;

	this->comboBox->clear();

	foreach (fairytale::ClipPackages::const_reference ref, packages)
	{
		this->comboBox->addItem(ref->name());
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
	return m_packages[this->comboBox->currentIndex()];
}
