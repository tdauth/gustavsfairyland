#ifndef CLIPEDITOR_H
#define CLIPEDITOR_H

#include <QtWidgets/QDialog>
#include <QTemporaryFile>

#include "recorder.h"
#include "ui_clipeditor.h"

class Clip;
class fairytale;
class LanguageDialog;

/**
 * \brief This editor dialog allows you to create a single clip.
 *
 * Provides widgets to modify the clip's properties.
 */
class ClipEditor : public QDialog, protected Ui::ClipEditor
{
	Q_OBJECT

	public slots:
		void clipIdChanged(const QString &text);
		void setIsPerson(bool isAPerson);
		void chooseImage();
		void captureImage();
		void chooseVideo();
		void recordVideo();
		void addNarratingSound();
		void removeNarratingSound();
		void addDescription();
		void removeDescription();

		QString execLanguageDialog();

	public:
		ClipEditor(fairytale *app, QWidget *parent);
		~ClipEditor();

		void fill(Clip *clip);

		void assignToClip(Clip *clip);

		/**
		 * Creates a newly allocated clip with parent \p parent.
		 * The clip has all the choosen properties.
		 * \return Returns a newly allocated clip object.
		 */
		Clip* clip(QObject *parent);

	private slots:
		void imageSaved(int id, const QString &fileName);
		void videoRecorderStateChanged(QMediaRecorder::State state);

	private:
		/**
		 * Checks if all required properties are set.
		 * \return Returns true if all required properties of the clip has been specified and the clip can be created. Otherwise it returns false.
		 */
		bool checkForValidFields();

		fairytale *m_app;
		Clip *m_clip;
		LanguageDialog *m_languageDialog;
		QString m_dir;

		Recorder m_recorder;
		QTemporaryFile m_file;
		bool m_recording;
};

#endif // CLIPEDITOR_H
