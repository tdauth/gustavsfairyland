#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QCamera>
#include <QMediaRecorder>
#include <QCameraImageCapture>
#include <QAudioRecorder>
#include <QCameraViewfinder>

/**
 * @brief The Recorder class allows recording videos or images.
 *
 * Recording videos and images directly with a webcam allows the users to create their own clips.
 *
 * @note Windows support: https://bugreports.qt.io/browse/QTBUG-29175
 */
class Recorder : public QObject
{
	public slots:
		void recordVideo(const QString &file);
		void recordImage(const QString &file);
		void recordAudio(const QString &file);
		void stopRecordingVideo();
		void stopRecordingAudio();

		void showCameraFinder(QWidget *parent);

	public:
		Recorder(QObject *object = nullptr);

		QMediaRecorder::State state() const;

		QMediaRecorder* recorder() const;

	private:
		QCamera *m_camera;
		QMediaRecorder *m_recorder;
		QCameraImageCapture *m_imageCapture;
		QAudioRecorder *m_audioRecorder;
		QCameraViewfinder *m_viewfinder;
};

inline QMediaRecorder::State Recorder::state() const
{
	return this->m_recorder->state();
}

inline QMediaRecorder* Recorder::recorder() const
{
	return this->m_recorder;
}

#endif // RECORDER_H
