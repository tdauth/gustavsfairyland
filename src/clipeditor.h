#ifndef CLIPEDITOR_H
#define CLIPEDITOR_H

#include <QDialog>
#include <QTemporaryFile>
#include <QDir>

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
		void chooseImage();
		void captureImage();
		void chooseVideo();
		void recordVideo();
		void addNarratingSound();
		void recordNarratingSound();
		void removeNarratingSound();
		void addDescription();
		void removeDescription();

		/**
		 * Shows a dialog to select the language for a property.
		 * \return Returns the language the user has selected.
		 */
		QString execLanguageDialog();

	public:
		ClipEditor(fairytale *app, QWidget *parent);
		~ClipEditor();

		/**
		 * Fills the dialog GUI elements with data from \p clip.
		 */
		void fill(Clip *clip);

		/**
		 * Assigns the data to \p clip.
		 */
		void assignToClip(Clip *clip);

		/**
		 * Creates a newly allocated clip with parent \p parent.
		 * The clip has all the choosen properties.
		 * \return Returns a newly allocated clip object.
		 */
		Clip* clip(QObject *parent);

		/**
		 * Moves all files of the clip to the current clips directory and makes their URLs relative to
		 * the application clips directory.
		 *
		 * This helps using the clip at different machines and separting it from the temporarily recorded data.
		 *
		 * \param parentDir The newly used parent directory of the clip's own directory.
		 * \param dirName The directory in \p parentDir into which the clip's files are moved.
		 */
		bool moveFilesToCurrentClipIdDir(const QDir parentDir, const QString dirName);

		/**
		 * The directory where a subdirectory with the clip's ID is created into which all the clip's files are moved.
		 *
		 * \{
		 */
		void setTargetClipsDirectory(const QDir &dir);
		QDir targetClipsDirectory() const;
		/**
		 * \}
		 */

	private slots:
		void clipIdChanged(const QString &text);
		void setIsPerson(bool isAPerson);

		void imageSaved(int id, const QString &fileName);
		void videoRecorderStateChanged(QMediaRecorder::State state);
		void audioRecorderStateChanged(QMediaRecorder::State state);

		void updateClipImage();
		void updateClipVideo();
		void updateClipNarratingSound();

		void onFinish(int result);

	private:
		/**
		 * Checks if all required properties are set.
		 * \return Returns true if all required properties of the clip has been specified and the clip can be created. Otherwise it returns false.
		 */
		bool checkForValidFields();

		bool deleteRecordedFile();

		QDir clipsDirectory() const;
		QDir currentClipDirectory() const;

		bool clipIdIsAlreadyUsed() const;
		bool moveFileToCurrentClipDir(const QDir parentDir, const QUrl &oldUrl, QUrl &newUrl);

		fairytale *m_app;
		Clip *m_clip;
		LanguageDialog *m_languageDialog;
		QString m_dir;

		/**
		 * The directory where the file is moved to when the dialog is accepted.
		 */
		QDir m_targetClipsDirectory;

		Recorder *m_recorder;
		QString m_recordedFile;

		/**
		 * Currently selected language by the language dialog.
		 */
		QString m_language;
};

inline void ClipEditor::setTargetClipsDirectory(const QDir &dir)
{
	this->m_targetClipsDirectory = dir;
}

inline QDir ClipEditor::targetClipsDirectory() const
{
	return this->m_targetClipsDirectory;
}

#endif // CLIPEDITOR_H
