#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QCamera>
#include <QMediaRecorder>
#include <QCameraImageCapture>
#include <QAudioRecorder>
#include <QCameraViewfinder>

class RecorderView;

/**
 * @brief The Recorder class allows recording videos or images.
 *
 * Recording videos and images directly with a webcam allows the users to create their own clips.
 *
 * @note Windows support: https://bugreports.qt.io/browse/QTBUG-29175
 */
class Recorder : public QObject
{
	Q_OBJECT

	public slots:
		void recordVideo();
		void recordImage();
		void recordAudio();
		void pauseRecordingVideo();
		void stopRecordingVideo();
		void stopRecordingAudio();

		void clearCameraFinder();

		RecorderView* showCameraFinder(QWidget *parent, QCamera::CaptureMode captureMode = QCamera::CaptureStillImage);

	public:
		Recorder(QObject *object = nullptr);

		QMediaRecorder::State state() const;

		QCamera* camera() const;
		QMediaRecorder* recorder() const;
		QCameraImageCapture* imageCapture() const;

		void setOutputFile(const QString &outputFile);
		QString outputFile() const;

	private:
		QCamera *m_camera;
		QMediaRecorder *m_recorder;
		QCameraImageCapture *m_imageCapture;
		QAudioRecorder *m_audioRecorder;
		QCameraViewfinder *m_viewfinder;
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

inline void Recorder::setOutputFile(const QString &outputFile)
{
	this->m_outputFile = outputFile;
}

inline QString Recorder::outputFile() const
{
	return this->m_outputFile;
}

#endif // RECORDER_H
