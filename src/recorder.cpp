#include <QtMultimedia>
#include <QtGui>
#include <QtWidgets>

#include "recorder.h"

void Recorder::recordVideo()
{
	qDebug() << "Camera:" << m_camera;
	qDebug() << "capture mode supported: " << m_camera->isCaptureModeSupported(QCamera::CaptureVideo);
	m_camera->start();
	qDebug() << "capture mode supported 2: " << m_camera->isCaptureModeSupported(QCamera::CaptureVideo);

	qDebug() << "Recording from:" << m_recorder->mediaObject();
	qDebug() << "Output file location:" << outputFile();
	m_recorder->setOutputLocation(QUrl::fromLocalFile(outputFile()));
	m_finshedRecording = false;
	m_recorder->record();
	m_isRecording = true;

	recordVideoPushButton->setText(tr("Pause Recording"));
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
	m_finshedRecording = true;
}

void Recorder::recordAudio()
{
	m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(outputFile()));
	m_finshedRecording = false;
	m_audioRecorder->record();
	m_isRecording = true;

	qDebug() << "Record audio to" << outputFile();

	recordAudioPushButton->setText(tr("Pause Recording"));
}

void Recorder::pauseRecordingVideo()
{
	m_recorder->pause();
	m_isRecording = false;

	recordVideoPushButton->setText(tr("Continue Recording"));
}

void Recorder::pauseRecordingAudio()
{
	m_audioRecorder->pause();
	m_isRecording = false;

	recordAudioPushButton->setText(tr("Continue Recording"));
}

void Recorder::stopRecordingVideo()
{
	qDebug() << "Stop recording video";
	m_recorder->stop();
	m_isRecording = false;

	recordVideoPushButton->setText(tr("Record Video"));
}

void Recorder::stopRecordingAudio()
{
	m_audioRecorder->stop();
	m_isRecording = false;

	recordAudioPushButton->setText(tr("Record Audio"));
}

void Recorder::stopAllRecording()
{
	stopRecordingVideo();
	stopRecordingAudio();
}

int Recorder::showCameraFinder(QCamera::CaptureMode captureMode, bool startRecording)
{
	// The result must not be Accepted!
	setResult(QDialog::Rejected);
	m_camera->setCaptureMode(captureMode);
	m_camera->start();

	m_cameraViewFinder->show();
	recordAudioPushButton->hide();

	setCameraCaptureMode(captureMode);

	if (captureMode == QCamera::CaptureVideo)
	{
		m_mode = RecordVideo;
		okPushButton->setFocus();
	}
	else
	{
		m_mode = CaptureImage;
		photoPushButton->setFocus();
	}

	this->showFullScreen();

	if (startRecording)
	{
		/*
		 * Make sure that recording from the camera or capturing an image is working and does not produce any
		 * error by waiting for the camera to become ready.
		 */
		waitUntilCameraIsReady();

		if (captureMode == QCamera::CaptureVideo)
		{
			recordVideo();
		}
		else
		{
			captureImage();
		}
	}

	const int result = this->exec();

	if (result == QDialog::Accepted)
	{
		qDebug() << "Waiting for recorded file";
		// Make absolutely sure the file exists, when this method returns.
		waitForRecordedFile(captureMode == QCamera::CaptureVideo);
	}

	stopAllRecording();
	// Stop the camera when everything is done.
	m_camera->stop();

	return result;
}

int Recorder::showAudioRecorder(bool startRecording)
{
	// The result must not be Accepted!
	setResult(QDialog::Rejected);

	m_cameraViewFinder->hide();
	okPushButton->show();
	cancelPushButton->show();
	photoPushButton->hide();
	recordVideoPushButton->hide();
	recordAudioPushButton->show();

	okPushButton->setFocus();

	m_mode = RecordAudio;

	this->showFullScreen();

	if (startRecording)
	{
		recordAudio();
	}

	const int result = this->exec();

	if (result == QDialog::Accepted)
	{
		qDebug() << "Waiting for recorded audio";
		// Make absolutely sure the file exists, when this method returns.
		waitForRecordedFile(true);
	}

	stopAllRecording();

	return result;
}

Recorder::Recorder(QWidget *parent) : QDialog(parent), m_camera(nullptr), m_recorder(nullptr), m_cameraViewFinder(new QCameraViewfinder(this)), m_finshedRecording(false), m_isRecording(false)
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
	const QStringList inputs = m_audioRecorder->audioInputs();
	const QString selectedInput = m_audioRecorder->defaultAudioInput();

	foreach (const QString &input, inputs)
	{
		const QString description = m_audioRecorder->audioInputDescription(input);
		// show descriptions to user and allow selection
		qDebug() << "Audio input" << input << description;
	}

	qDebug() << "Selected audio input" << selectedInput;

	foreach (const QString &codecName, m_audioRecorder->supportedAudioCodecs())
	{
		qDebug() << "Audio codec" << codecName;
	}

	qDebug() << "Selected audio codec" << m_audioRecorder->audioSettings().codec();

	m_camera->setViewfinder(m_cameraViewFinder);

	setupUi(this);

	verticalLayout->replaceWidget(cameraFinderWidget, m_cameraViewFinder);
	m_cameraViewFinder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	connect(m_recorder, &QMediaRecorder::stateChanged, this, &Recorder::videoRecorderStateChanged);
	connect(m_audioRecorder, &QMediaRecorder::stateChanged, this, &Recorder::audioRecorderStateChanged);

	connect(photoPushButton, &QPushButton::clicked, this, &Recorder::pressCapturePhoto);
	connect(recordVideoPushButton, &QPushButton::clicked, this, &Recorder::pressRecordVideo);
	connect(recordAudioPushButton, &QPushButton::clicked, this, &Recorder::pressRecordAudio);
	connect(stopPushButton, &QPushButton::clicked, this, &Recorder::pressStopRecording);
	connect(okPushButton, &QPushButton::clicked, this, &Recorder::accept);
	connect(cancelPushButton, &QPushButton::clicked, this, &Recorder::reject);

	/*
	 * Make sure that all recording is finished after setting the state to rejected or accepted.
	 */
	connect(this, &QDialog::finished, this, &Recorder::stopAllRecording);
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

void Recorder::videoRecorderStateChanged(QMediaRecorder::State state)
{
	qDebug() << "Video recorder state changed in recorder";

	switch (state)
	{
		case QMediaRecorder::StoppedState:
		{
			m_isRecording = false;
			m_finshedRecording = true;

			break;
		}

		case QMediaRecorder::PausedState:
		{
			m_isRecording = false;

			break;
		}

		case QMediaRecorder::RecordingState:
		{
			m_isRecording = true;

			break;
		}
	}
}

void Recorder::audioRecorderStateChanged(QMediaRecorder::State state)
{
	qDebug() << "Audio recorder state changed" << state;

	switch (state)
	{
		case QMediaRecorder::StoppedState:
		{
			m_isRecording = false;
			m_finshedRecording = true;

			break;
		}

		case QMediaRecorder::PausedState:
		{
			m_isRecording = false;

			break;
		}

		case QMediaRecorder::RecordingState:
		{
			m_isRecording = true;

			break;
		}
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
	if (m_isRecording)
	{
		pauseRecordingVideo();
	}
	else
	{
		recordVideo();
	}
}

void Recorder::pressRecordAudio()
{
	qDebug() << "Pressed record audio";

	if (m_isRecording)
	{
		pauseRecordingAudio();
	}
	else
	{
		recordAudio();
	}
}

void Recorder::pressStopRecording()
{
	stopAllRecording();
}

void Recorder::hideEvent(QHideEvent *event)
{
	QDialog::hideEvent(event);
}

void Recorder::waitUntilCameraIsReady()
{
	QEventLoop eventLoop(this);

	while (m_camera->status() != QCamera::ActiveStatus)
	{
		eventLoop.processEvents(QEventLoop::AllEvents, 1000);
	}

	qDebug() << "Camera status is " << m_camera->status();
	eventLoop.quit();
}

void Recorder::waitForRecordedFile(bool videoOrAudio)
{
	QEventLoop eventLoop(this);

	while (!m_finshedRecording && ((videoOrAudio && m_recorder->error() == QMediaRecorder::NoError) || !videoOrAudio))
	{
		eventLoop.processEvents(QEventLoop::AllEvents, 1000);
	}

	qDebug() << "Recorded file is " << m_finshedRecording << "and error is " << m_recorder->error() << "and status is" << m_recorder->status();
	eventLoop.quit();

	m_finshedRecording = false;
}

#include "moc_recorder.cpp"
