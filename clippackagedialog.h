#ifndef CLIPPACKAGEDIALOG_H
#define CLIPPACKAGEDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_clippackagedialog.h"

#include "fairytale.h"

/**
 * \brief Allows the selection of a single clip package from a list of clip packages.
 */
class ClipPackageDialog : public QDialog, protected Ui::ClipPackageDialog
{
	Q_OBJECT

	public:
		ClipPackageDialog(QWidget* parent, Qt::WindowFlags f = 0);

		/**
		 * Fills the combo box with \p packages for selection.
		 */
		void fill(const fairytale::ClipPackages &packages);

		/**
		 * \return Returns the selected clip package. Returns nullptr if the package list is empty.
		 */
		ClipPackage* clipPackage() const;

	private:
		fairytale::ClipPackages m_packages;
};

#endif // CLIPPACKAGEDIALOG_H
