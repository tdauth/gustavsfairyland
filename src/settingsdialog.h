#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QUrl>
#include <QSettings>

#include "fairytale.h"
#include "translatedwidget.h"
#include "ui_settings.h"

class ClipPackage;
class Clip;
class BonusClip;

/**
 * \brief Allows to change the settings of the game and apply them or cancel.
 */
class SettingsDialog : public TranslatedWidget, protected Ui::SettingsDialog
{
	Q_OBJECT

	public slots:
		/**
		 * Restores the default settings but does not apply them.
		 */
		void restoreDefaults();
		/**
		 * Shows a file dialog to change the clips directory.
		 */
		void changeClipsDirectory();
		/**
		 * Applies the settings.
		 */
		void apply();
		/**
		 * Updates all GUI elements from the settings dialog with the current settings of the game.
		 */
		void update();

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

		/**
		 * Checks the clips group box and shows the inner widget.
		 */
		void showClipsGroupBox();

	private slots:
		void itemDoubleClicked(QTreeWidgetItem *item, int column);

	public:
		SettingsDialog(fairytale *app, QWidget *parent);

		void fill(const fairytale::ClipPackages &packages);
		void fill(ClipPackage *package);

		void load(QSettings &settings);
		void save(QSettings &settings);

		virtual void retranslateUi(QWidget *widget) override
		{
			Ui::SettingsDialog::retranslateUi(widget);
		}

	private:
		QUrl m_clipsDir;
		typedef QMap<QTreeWidgetItem*, ClipPackage*> ClipPackages;
		ClipPackages m_clipPackages;
		typedef QMap<QTreeWidgetItem*, Clip*> Clips;
		Clips m_clips;
		typedef QMap<QTreeWidgetItem*, BonusClip*> BonusClips;
		BonusClips m_bonusClips;
};

#endif // SETTINGSDIALOG_H