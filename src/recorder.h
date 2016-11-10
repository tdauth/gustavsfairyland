#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QMediaRecorder>
#include <QCameraImageCapture>

/**
 * @brief The Recorder class allows recording videos or images.
 *
 * @note Windows support: https://bugreports.qt.io/browse/QTBUG-29175
 */
class Recorder : public QObject
{
	public slots:
		void recordVideo(const QString &file);
		void recordImage(const QString &file);
		void stopRecordingVideo();

	public:
		Recorder(QObject *object = nullptr);

		QMediaRecorder::State state() const;

		QMediaRecorder* recorder() const;

	private:
		QMediaRecorder *m_recorder;
		QCameraImageCapture *m_imageCapture;
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
