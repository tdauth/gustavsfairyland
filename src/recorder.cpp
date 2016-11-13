#include <QtMultimedia>
#include <QtGui>
#include <QtWidgets>

#include "recorder.h"

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

void Recorder::captureImage()
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

void Recorder::pauseRecordingAudio()
{
	m_audioRecorder->pause();
}

void Recorder::stopRecordingVideo()
{
	m_recorder->stop();
}

void Recorder::stopRecordingAudio()
{
	m_audioRecorder->stop();
}

void Recorder::stopAllRecording()
{
	if (m_recorder->state() != QMediaRecorder::State::StoppedState)
	{
		m_recorder->stop();
	}

	if (m_audioRecorder->state() != QMediaRecorder::State::StoppedState)
	{
		m_audioRecorder->stop();
	}
}

void Recorder::clearCameraFinder()
{
	//m_camera->stop();
	//m_camera->setViewfinder((QVideoWidget*)nullptr);
}

int Recorder::showCameraFinder(QCamera::CaptureMode captureMode)
{
	// The result must not be Accepted!
	setResult(QDialog::Rejected);
	m_camera->setCaptureMode(captureMode);
	m_camera->start();

	m_cameraViewFinder->show();
	recordAudioPushButton->hide();

	setCameraCaptureMode(captureMode);

	return this->exec();
}

int Recorder::showAudioRecorder()
{
	// The result must not be Accepted!
	setResult(QDialog::Rejected);

	m_cameraViewFinder->hide();
	photoPushButton->hide();
	recordVideoPushButton->hide();
	recordAudioPushButton->show();

	return this->exec();
}

Recorder::Recorder(QWidget *parent) : QDialog(parent), m_camera(nullptr), m_recorder(nullptr), m_cameraViewFinder(new QCameraViewfinder(this))
{
	const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

	foreach (const QCameraInfo &cameraInfo, cameras)
	{
		if (m_camera == nullptr)
		{
			m_camera = new QCamera(cameraInfo);
		}

		qDebug() << "Info:" << cameraInfo << "Camera:" << m_camera;
	}

	m_recorder = new QMediaRecorder(m_camera);
	m_imageCapture = new QCameraImageCapture(m_camera);

	m_audioRecorder = new QAudioRecorder(this);
	QStringList inputs = m_audioRecorder->audioInputs();
	QString selectedInput = m_audioRecorder->defaultAudioInput();

	foreach (QString input, inputs)
	{
		QString description = m_audioRecorder->audioInputDescription(input);
		// show descriptions to user and allow selection
		selectedInput = input;
		qDebug() << "Audio input" << input;
	}

	m_audioRecorder->setAudioInput(selectedInput);

	QAudioEncoderSettings audioSettings;
	audioSettings.setCodec("audio/amr");
	audioSettings.setQuality(QMultimedia::HighQuality);

	m_audioRecorder->setEncodingSettings(audioSettings);

	m_camera->setViewfinder(m_cameraViewFinder);

	setupUi(this);

	verticalLayout->replaceWidget(widget, m_cameraViewFinder);

	connect(photoPushButton, &QPushButton::clicked, this, &Recorder::pressCapturePhoto);
	connect(recordVideoPushButton, &QPushButton::clicked, this, &Recorder::pressRecordVideo);
	connect(recordAudioPushButton, &QPushButton::clicked, this, &Recorder::pressRecordAudio);
	connect(stopPushButton, &QPushButton::clicked, this, &Recorder::pressStopRecording);
	connect(okPushButton, &QPushButton::clicked, this, &Recorder::accept);
	connect(cancelPushButton, &QPushButton::clicked, this, &Recorder::reject);
}

void Recorder::setCameraCaptureMode(QCamera::CaptureMode captureMode)
{
	if (captureMode == QCamera::CaptureStillImage)
	{
		okPushButton->hide();
		photoPushButton->show();
		photoPushButton->setText(tr("Make Photo"));
		recordVideoPushButton->hide();
		stopPushButton->hide();
	}
	else if (captureMode == QCamera::CaptureVideo)
	{
		okPushButton->show();
		photoPushButton->hide();
		recordVideoPushButton->show();
		recordVideoPushButton->setText(tr("Record Video"));
		stopPushButton->show();
	}
}

void Recorder::pressCapturePhoto()
{
	setResult(QDialog::Accepted);
	captureImage();

	accept();
}

void Recorder::pressRecordVideo()
{
	// TODO store state manually as flag since the calls are asynchronus-
	if (m_recorder->state() == QMediaRecorder::RecordingState)
	{
		recordVideoPushButton->setText(tr("Continue Recording"));
		pauseRecordingVideo();
	}
	else
	{
		recordVideoPushButton->setText(tr("Pause Recording"));
		recordVideo();
	}
}

void Recorder::pressRecordAudio()
{
	// TODO store state manually as flag since the calls are asynchronus-
	if (m_audioRecorder->state() == QMediaRecorder::RecordingState)
	{
		recordAudioPushButton->setText(tr("Continue Recording"));
		pauseRecordingAudio();
	}
	else
	{
		recordAudioPushButton->setText(tr("Pause Recording"));
		recordAudio();
	}
}

void Recorder::pressStopRecording()
{
	stopAllRecording();
}

void Recorder::closeEvent(QCloseEvent *event)
{
	qDebug() << "Stopping all recorders";
	this->stopAllRecording();
	m_camera->stop();
	QDialog::closeEvent(event);
}

#include "moc_recorder.cpp"