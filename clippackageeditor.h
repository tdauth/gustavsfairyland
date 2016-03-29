#ifndef CLIPPACKAGEEDITOR_H
#define CLIPPACKAGEEDITOR_H

#include <QtWidgets/QDialog>

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
		/**
		 * Opens a \ref ClipEditor dialog to create a new clip.
		 */
		void addClip();
		void loadPackage();
		void saveAs();

	public:
		ClipPackageEditor(fairytale *app, QWidget *parent);

		fairytale* app() const;

	private:
		fairytale *m_app;
		ClipEditor *m_clipEditor;
		ClipPackage *m_clipPackage;
};

inline fairytale* ClipPackageEditor::app() const
{
	return this->m_app;
}

#endif // CLIPPACKAGEEDITOR_H
