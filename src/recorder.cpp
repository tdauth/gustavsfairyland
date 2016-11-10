#include <QtMultimedia>
#include <QCameraViewfinder>

#include "recorder.h"

void Recorder::record(const QString &file)
{
	qDebug() << "Recording from:" << (QCamera*)m_recorder->mediaObject();
	m_recorder->setOutputLocation(QUrl::fromLocalFile(file));
	m_recorder->record();
}

void Recorder::stop()
{
	m_recorder->stop();
}

Recorder::Recorder(QObject *parent) : QObject(parent), m_recorder(nullptr)
{
	QCamera *camera = nullptr;
	QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

	foreach (const QCameraInfo &cameraInfo, cameras) {
		if (cameraInfo.deviceName() == "/dev/video0"){
			camera = new QCamera(cameraInfo);
		}
		qDebug() << "Info:" << cameraInfo << "Camera:" << camera;
	}

	m_recorder = new QMediaRecorder(camera);

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

	camera->setCaptureMode(QCamera::CaptureVideo);
	camera->start();

	QCameraViewfinder *viewfinder = new QCameraViewfinder();
	viewfinder->show();

	camera->setViewfinder(viewfinder);

}
