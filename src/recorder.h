#ifndef GUSTAV_RECORDER_H
#define GUSTAV_RECORDER_H

#include <QDialog>
#include <QCamera>
#include <QMediaRecorder>
#include <QCameraImageCapture>
#include <QAudioRecorder>
#include <QCameraViewfinder>

#include "config.h"

#ifdef USE_QTMEL
#include <QLabel>

#include <CameraGrabber>
#include <AudioGrabber>
#include <AudioCodecSettings>
#include <VideoCodecSettings>
#include <Encoder>
#include <Recorder>
#endif

#include "ui_recorder.h"

#ifdef USE_QTMEL
typedef Recorder QtMELRecorder;
#endif

namespace gustav
{

/**
 * \brief The Recorder class allows recording videos or images.
 *
 * Recording videos and images directly with a webcam allows the users to create their own clips.
 *
 * \note Windows support: https://bugreports.qt.io/browse/QTBUG-29175
 */
class Recorder : public QDialog, protected Ui::Recorder
{
	Q_OBJECT

	private slots:
#ifdef USE_QTMEL
		void videoRecorderStateChangedQtMEL(QtMELRecorder::State state);

		void onEncoderError(Encoder::Error error);
		void onGrabberError(AbstractGrabber::Error error);
#endif
		void videoRecorderStateChanged(QMediaRecorder::State state);
		void audioRecorderStateChanged(QMediaRecorder::State state);

		void pressCapturePhoto();
		/**
		 * Pauses if already recoridng.
		 */
		void pressRecordVideo();
		void pressRecordAudio();
		void pressStopRecording();

#ifdef USE_QTMEL
		void showFrame(const QImage &frame);
#endif

	public slots:
		void recordVideo();
		void captureImage();
		void recordAudio();
		void pauseRecordingVideo();
		void pauseRecordingAudio();
		void stopRecordingVideo();
		void stopRecordingAudio();
		void stopAllRecording();

		/**
		 * Shows a camera finder widget which has the image of the currently recording camera.
		 * This is a blocking method returning when the dialog is closed and the file is recorded (if accepted).
		 * \param captureMode The capture mode which is used for the camera.
		 * \param startRecording If this value is true the recording starts immediately. Otherwise the user has to press the record button.
		 * \return Returns the result of exec().
		 */
		int showCameraFinder(QCamera::CaptureMode captureMode = QCamera::CaptureStillImage, bool startRecording = false);
		/**
		 * \return Returns the result of exec().
		 */
		int showAudioRecorder(bool startRecording = false);

	public:
		enum Mode
		{
			RecordVideo,
			RecordAudio,
			CaptureImage
		};

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
		Mode mode() const;

	protected:
		virtual void hideEvent(QHideEvent *event) override;

	private:
		void waitUntilCameraIsReady();
		void waitForRecordedFile(bool videoOrAudio);

		QCamera *m_camera;
		QMediaRecorder *m_recorder;
		QCameraImageCapture *m_imageCapture;
		QAudioRecorder *m_audioRecorder;
		QCameraViewfinder *m_cameraViewFinder;
		QString m_outputFile;

#ifdef USE_QTMEL
		CameraGrabber *m_cameraGrabber;
		AudioGrabber *m_audioGrabber;
		QtMELRecorder *m_qtmelRecorder;
		QLabel *m_frameLabel;
#endif

		bool m_finshedRecording;
		bool m_isRecording;
		Mode m_mode;
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

inline Recorder::Mode Recorder::mode() const
{
	return this->m_mode;
}

}

#endif // GUSTAV_RECORDER_H
