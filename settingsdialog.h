#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QUrl>

#include "ui_settings.h"

class fairytale;

/**
 * @brief Allows to change the settings of the game and apply them or cancel.
 */
class SettingsDialog : public QDialog, protected Ui::SettingsDialog
{
	Q_OBJECT

	public slots:
		void changeClipsDirectory();
		void apply();
		void update();

	public:
		SettingsDialog(fairytale *app, QWidget *parent);

	private:
		fairytale *m_app;
		QUrl m_clipsDir;
};

#endif // SETTINGSDIALOG_H
