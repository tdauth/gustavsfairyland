#ifndef CLIPSDIALOG_H
#define CLIPSDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_clips.h"
#include "fairytale.h"

class fairytale;
class ClipPackage;
class Clip;
class BonusClip;

/**
 * \brief Lists all clip packages which are used and allows the user to add and remove them.
 */
class ClipsDialog : public QDialog, protected Ui::ClipsWidget
{
	Q_OBJECT

	public slots:
		/**
		 * Lets the user select a clips.xml file which will be added as package to the list of clip packages.
		 */
		void addFile();
		void addDirectory();
		/**
		 * Removes the currently selected package from the game. This does not delete any file data.
		 * It just removes the package and makes it unavailable as game.
		 */
		void removeSelected();

	private slots:
		void itemDoubleClicked(QTreeWidgetItem *item, int column);

	public:
		ClipsDialog(fairytale *app, QWidget *parent);

		void fill(const fairytale::ClipPackages &packages);
		void fill(ClipPackage *package);

	private:
		fairytale *m_app;
		typedef QMap<QTreeWidgetItem*, ClipPackage*> ClipPackages;
		ClipPackages m_clipPackages;
		typedef QMap<QTreeWidgetItem*, Clip*> Clips;
		Clips m_clips;
		typedef QMap<QTreeWidgetItem*, BonusClip*> BonusClips;
		BonusClips m_bonusClips;
};

#endif // CLIPSDIALOG_H
