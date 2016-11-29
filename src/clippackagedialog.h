#ifndef CLIPPACKAGEDIALOG_H
#define CLIPPACKAGEDIALOG_H

#include <QDialog>

#include "ui_clippackagedialog.h"

#include "fairytale.h"

/**
 * \brief Allows the selection of a single clip package from a list of clip packages.
 */
class ClipPackageDialog : public QDialog, protected Ui::ClipPackageDialog
{
	Q_OBJECT

	public:
		ClipPackageDialog(QWidget *parent, Qt::WindowFlags f = 0);

		/**
		 * Fills the combo box with \p packages for selection.
		 */
		void fill(const fairytale::ClipPackages &packages, const fairytale::GameModes &gameModes, fairytale *app);

		/**
		 * \return Returns the selected clip packages.
		 */
		fairytale::ClipPackages clipPackages() const;
		/**
		 * \return Returns the selected clip package. Returns nullptr if the package list is empty.
		 */
		GameMode* gameMode() const;
		fairytale::Difficulty difficulty() const;
		int maxRounds() const;
		bool useMaxRounds() const;

	private slots:
		void currentGameModeIndexChanged(int index);
		void validate();

	private:
		fairytale *m_app;
		fairytale::ClipPackages m_packages;
		fairytale::GameModes m_gameModes;
		QList<QCheckBox*> m_checkBoxes;
};

#endif // CLIPPACKAGEDIALOG_H
