#ifndef CLIPPACKAGEDIALOG_H
#define CLIPPACKAGEDIALOG_H

#include <qt5/QtWidgets/QDialog>

#include "ui_clippackagedialog.h"

#include "fairytale.h"

class ClipPackageDialog : public QDialog, protected Ui::ClipPackageDialog
{
	Q_OBJECT

	public:
		ClipPackageDialog(QWidget* parent, Qt::WindowFlags f = 0);

		void fill(const fairytale::ClipPackages &packages);

		ClipPackage* clipPackage() const;

	private:
		fairytale::ClipPackages m_packages;
};

#endif // CLIPPACKAGEDIALOG_H
