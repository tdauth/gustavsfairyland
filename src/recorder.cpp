#include <QtMultimedia>
#include <QtGui>
#include <QtWidgets>

#include "recorder.h"
#include "recorderview.h"

void Recorder::recordVideo()
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
	m_camera->start();
	qDebug() << "capture mode supported 2: " << m_camera->isCaptureModeSupported(QCamera::CaptureVideo);

	qDebug() << "Recording from:" << m_recorder->mediaObject();
	m_recorder->setOutputLocation(QUrl::fromLocalFile(outputFile()));
	m_recorder->record();
}

void Recorder::recordImage()
{
	//m_camera->setCaptureMode(QCamera::CaptureStillImage);
	//m_camera->start();
	//on half pressed shutter button
	m_camera->searchAndLock();

	//m_imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
	/* TODO when setting manually set everything
	QImageEncoderSettings encoderSettings = m_imageCapture->encodingSettings();
	encoderSettings.setQuality(QMultimedia::VeryHighQuality);
	//encoderSettings.setCodec("jpeg");
	qDebug() << "Resolution" << encoderSettings.resolution();
	qDebug() << "Codec" << encoderSettings.codec();
	m_imageCapture->setEncodingSettings(encoderSettings);
	*/
	const int id = m_imageCapture->capture(outputFile());
	qDebug() << "Recording image to " << outputFile() << "with id" << id;

	//on shutter button released
	m_camera->unlock();
}

void Recorder::recordAudio()
{
	m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(outputFile()));
	m_audioRecorder->record();
}

void Recorder::pauseRecordingVideo()
{
	m_recorder->pause();
}

void Recorder::stopRecordingVideo()
{
	m_recorder->stop();
}

void Recorder::stopRecordingAudio()
{
	m_audioRecorder->stop();
}

void Recorder::clearCameraFinder()
{
	m_camera->stop();
	//m_camera->setViewfinder((QVideoWidget*)nullptr);
}

RecorderView* Recorder::showCameraFinder(QWidget *parent, QCamera::CaptureMode captureMode)
{
	RecorderView *recorderView = new RecorderView(parent, this);
	m_camera->setViewfinder(recorderView->cameraViewFinder());
	m_camera->setCaptureMode(captureMode);
	m_camera->start();

	recorderView->setCameraCaptureMode(captureMode);

	if (captureMode == QCamera::CaptureStillImage)
	{
		connect(recorderView, &RecorderView::onPressPhotoButton, this, &Recorder::recordImage);
	}
	else if (captureMode == QCamera::CaptureVideo)
	{
		connect(recorderView, &RecorderView::onPressPhotoButton, this, &Recorder::recordVideo);
		connect(recorderView, &RecorderView::onPressPauseButton, this, &Recorder::pauseRecordingVideo);
		connect(recorderView, &RecorderView::onPressStopButton, this, &Recorder::stopRecordingVideo);
	}

	return recorderView;
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

#include "moc_recorder.cpp"