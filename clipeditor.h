#ifndef CLIPEDITOR_H
#define CLIPEDITOR_H

#include <QtWidgets/QDialog>

#include "ui_clipeditor.h"

class Clip;

/**
 * \brief This editor dialog allows you to create a single clip.
 *
 * Provides widgets to modify the clip's properties.
 */
class ClipEditor : public QDialog, protected Ui::ClipEditor
{
	Q_OBJECT

	public slots:
		void descriptionChanged(const QString &description);
		void setIsPerson(bool isAPerson);
		void chooseImage();
		void chooseVideo();
		void chooseNarratorVideo();

	public:
		ClipEditor(QWidget *parent);
		~ClipEditor();

		/**
		 * Creates a newly allocated clip with parent \p parent.
		 * The clip has all the choosen properties.
		 * \return Returns a newly allocated clip object.
		 */
		Clip* clip(QObject *parent);

	private:
		/**
		 * Checks if all required properties are set.
		 * \return Returns true if all required properties of the clip has been specified and the clip can be created. Otherwise it returns false.
		 */
		bool checkForValidFields();

		Clip *m_clip;
		QString m_dir;
};

#endif // CLIPEDITOR_H
