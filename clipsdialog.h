#ifndef CLIPSDIALOG_H
#define CLIPSDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_clips.h"
#include "fairytale.h"

class fairytale;
class Clip;

class ClipsDialog : public QDialog, protected Ui::ClipsWidget
{
	Q_OBJECT

	public slots:
		void addFile();
		void addDirectory();

	private slots:
		void itemDoubleClicked(QTreeWidgetItem *item, int column);

	public:
		ClipsDialog(fairytale *app, QWidget *parent);

		void fill(const fairytale::ClipPackages &packages);
		void fill(ClipPackage *package);

	private:
		fairytale *m_app;
		typedef QMap<QTreeWidgetItem*, Clip*> Clips;
		Clips m_clips;
};

#endif // CLIPSDIALOG_H
