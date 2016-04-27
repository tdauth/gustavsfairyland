#ifndef GAMEMODEDIALOG_H
#define GAMEMODEDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_gamemodedialog.h"

#include "fairytale.h"

/**
 * \brief Allows the selection of a single clip package from a list of clip packages.
 */
class GameModeDialog : public QDialog, protected Ui::GameModeDialog
{
	Q_OBJECT

	public:
		GameModeDialog(QWidget* parent, Qt::WindowFlags f = 0);

		/**
		 * Fills the combo box with \p packages for selection.
		 */
		void fill(const fairytale::GameModes &gameModes);

		/**
		 * \return Returns the selected clip package. Returns nullptr if the package list is empty.
		 */
		GameMode* gameMode() const;

	private:
		fairytale::GameModes m_gameModes;
};

#endif // GAMEMODEDIALOG_H
