#ifndef CLIPPACKAGEEDITOR_H
#define CLIPPACKAGEEDITOR_H

#include <QDialog>
#include <QTemporaryDir>

#include "ui_clippackageeditor.h"

class ClipEditor;
class ClipPackage;
class fairytale;

/**
 * \brief The clip package editor allows you to create your own clip packages or edit existing ones.
 *
 * Clip packages contain one or several clips in a single file.
 */
class ClipPackageEditor : public QDialog, protected Ui::ClipPackageEditor
{
    Q_OBJECT

	public slots:
		void idChanged(const QString &text);
		void loadCustomClipsPackage();
		/**
		 * Opens a \ref ClipEditor dialog to create a new clip.
		 */
		void addClip();
		void editClip();
		void removeClip();
		void newPackage();
		void loadPackage();
		void saveAs();
		void save();
		void closePackage();

	private slots:
		void changedCurrentClipItem(QTreeWidgetItem *current, QTreeWidgetItem *previous);
		void changedCurrentBonusClipItem(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	public:
		ClipPackageEditor(fairytale *app, QWidget *parent);
		~ClipPackageEditor();

		fairytale* app() const;

		ClipEditor* clipEditor();

	private:
		bool checkForValidFields();
		void loadClipPackage();

		fairytale *m_app;
		ClipEditor *m_clipEditor;
		ClipPackage *m_clipPackage;
		QString m_dir;

		/**
		 * A temporary directory which is used for extraction of all clips.
		 */
		typedef QScopedPointer<QTemporaryDir> TmpDir;
		TmpDir m_extractDir;
};

inline fairytale* ClipPackageEditor::app() const
{
	return this->m_app;
}

#endif // CLIPPACKAGEEDITOR_H