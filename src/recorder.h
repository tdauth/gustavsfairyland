#ifndef RECORDER_H
#define RECORDER_H

#include <QDialog>
#include <QCamera>
#include <QMediaRecorder>
#include <QCameraImageCapture>
#include <QAudioRecorder>
#include <QCameraViewfinder>

#include "ui_recorder.h"

/**
 * @brief The Recorder class allows recording videos or images.
 *
 * Recording videos and images directly with a webcam allows the users to create their own clips.
 *
 * @note Windows support: https://bugreports.qt.io/browse/QTBUG-29175
 */
class Recorder : public QDialog, protected Ui::Recorder
{
	Q_OBJECT

	private slots:
		void pressCapturePhoto();
		/**
		 * Pauses if already recoridng.
		 */
		void pressRecordVideo();
		void pressRecordAudio();
		void pressStopRecording();

	public slots:
		void recordVideo();
		void captureImage();
		void recordAudio();
		void pauseRecordingVideo();
		void pauseRecordingAudio();
		void stopRecordingVideo();
		void stopRecordingAudio();
		void stopAllRecording();

		void clearCameraFinder();

		int showCameraFinder(QCamera::CaptureMode captureMode = QCamera::CaptureStillImage);
		int showAudioRecorder();

	public:
		Recorder(QWidget *parent = nullptr);

		QMediaRecorder::State state() const;

		QCamera* camera() const;
		QMediaRecorder* recorder() const;
		QCameraImageCapture* imageCapture() const;
		QAudioRecorder* audioRecorder() const;

		void setOutputFile(const QString &outputFile);
		QString outputFile() const;

		void setCameraCaptureMode(QCamera::CaptureMode captureMode);

		QCameraViewfinder* cameraViewFinder() const;

	protected:
		virtual void closeEvent(QCloseEvent *event) override;

	private:
		QCamera *m_camera;
		QMediaRecorder *m_recorder;
		QCameraImageCapture *m_imageCapture;
		QAudioRecorder *m_audioRecorder;
		QCameraViewfinder *m_cameraViewFinder;
		QString m_outputFile;
};

inline QMediaRecorder::State Recorder::state() const
{
	return this->m_recorder->state();
}

inline QCamera* Recorder::camera() const
{
	return this->m_camera;
}

inline QMediaRecorder* Recorder::recorder() const
{
	return this->m_recorder;
}

inline QCameraImageCapture* Recorder::imageCapture() const
{
	return this->m_imageCapture;
}

inline QAudioRecorder* Recorder::audioRecorder() const
{
	return this->m_audioRecorder;
}

inline void Recorder::setOutputFile(const QString &outputFile)
{
	this->m_outputFile = outputFile;
}

inline QString Recorder::outputFile() const
{
	return this->m_outputFile;
}

#endif // RECORDER_H
