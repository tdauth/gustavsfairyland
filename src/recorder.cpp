#include <QtMultimedia>
#include <QCameraViewfinder>

#include "recorder.h"

void Recorder::recordVideo(const QString &file)
{

	// NOTE On Linux the encoder settings have to be supported by GStreamer.
	// gstreamer0.10-plugins-bad gstreamer0.10-plugins-ugly
	/*
	QAudioEncoderSettings audioSettings;
	audioSettings.setCodec("audio/amr");
	audioSettings.setQuality(QMultimedia::HighQuality);

	m_recorder->setAudioSettings(audioSettings);

	QVideoEncoderSettings videoSettings;
	videoSettings.setCodec("video/mpeg2");
	videoSettings.setResolution(640, 480);

	m_recorder->setVideoSettings(videoSettings);
	*/

	qDebug() << "Camera:" << m_camera;
	qDebug() << "capture mode supported: " << m_camera->isCaptureModeSupported(QCamera::CaptureVideo);
	m_camera->setCaptureMode(QCamera::CaptureVideo);
	m_camera->start();
	qDebug() << "capture mode supported 2: " << m_camera->isCaptureModeSupported(QCamera::CaptureVideo);

	qDebug() << "Recording from:" << m_recorder->mediaObject();
	m_recorder->setOutputLocation(QUrl::fromLocalFile(file));
	m_recorder->record();
}

void Recorder::recordImage(const QString &file)
{
	m_camera->setCaptureMode(QCamera::CaptureStillImage);
	m_camera->start();
	//on half pressed shutter button
	m_camera->searchAndLock();

	m_imageCapture->capture(file);

	//on shutter button released
	m_camera->unlock();
}

void Recorder::recordAudio(const QString &file)
{
	m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(file));
	m_audioRecorder->record();
}

void Recorder::stopRecordingVideo()
{
	m_recorder->stop();
}

void Recorder::stopRecordingAudio()
{
	m_audioRecorder->stop();
}

void Recorder::showCameraFinder(QWidget *parent)
{
	m_viewfinder->show();
	m_camera->setViewfinder(m_viewfinder);
}

Recorder::Recorder(QObject *parent) : QObject(parent), m_camera(nullptr), m_recorder(nullptr)
{
	QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

	foreach (const QCameraInfo &cameraInfo, cameras) {
		if (/*cameraInfo.deviceName() == "/dev/video0"*/ m_camera == nullptr){
			m_camera = new QCamera(cameraInfo);
		}
		qDebug() << "Info:" << cameraInfo << "Camera:" << m_camera;
	}

	m_recorder = new QMediaRecorder(m_camera);
	m_imageCapture = new QCameraImageCapture(m_camera);

	m_audioRecorder = new QAudioRecorder();
	QStringList inputs = m_audioRecorder->audioInputs();
	QString selectedInput = m_audioRecorder->defaultAudioInput();

	foreach (QString input, inputs) {
		QString description = m_audioRecorder->audioInputDescription(input);
		// show descriptions to user and allow selection
		selectedInput = input;
	}

	m_audioRecorder->setAudioInput(selectedInput);

	QAudioEncoderSettings audioSettings;
	audioSettings.setCodec("audio/amr");
	audioSettings.setQuality(QMultimedia::HighQuality);

	m_audioRecorder->setEncodingSettings(audioSettings);

	m_viewfinder = new QCameraViewfinder();
}
