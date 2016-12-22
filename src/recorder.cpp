#include <QtMultimedia>
#include <QtGui>
#include <QtWidgets>

#include "recorder.h"

namespace gustav
{

void Recorder::recordVideo()
{
#ifdef USE_QTMEL
	m_cameraGrabber->setDeviceIndex(0);
	m_qtmelRecorder->encoder()->setVideoSize(CameraGrabber::maximumFrameSize(m_cameraGrabber->deviceIndex()));

	m_qtmelRecorder->encoder()->setFilePath(outputFile() + "video.avi");
	qDebug() << "Encoder file path:" << m_qtmelRecorder->encoder()->filePath();

	connect(m_cameraGrabber, &CameraGrabber::frameAvailable, this, &Recorder::showFrame);

	//m_qtmelRecorder->encoder()->start(); // has to be called before but is done automatically in m_qtmelRecorder->start()
	m_qtmelRecorder->start();
	qDebug() << "Starting QtMEL video recording";
#else
	qDebug() << "Camera:" << m_camera;
	qDebug() << "capture mode supported: " << m_camera->isCaptureModeSupported(QCamera::CaptureVideo);
	m_camera->start();
	qDebug() << "capture mode supported 2: " << m_camera->isCaptureModeSupported(QCamera::CaptureVideo);

	qDebug() << "Recording from:" << m_recorder->mediaObject();
	qDebug() << "Output file location:" << outputFile();
	m_recorder->setOutputLocation(QUrl::fromLocalFile(outputFile()));
	m_recorder->record();
#endif

	m_finshedRecording = false;
	m_isRecording = true;

	recordVideoPushButton->setText(tr("Pause Recording"));
}

void Recorder::captureImage()
{
	// on half pressed shutter button
	m_camera->searchAndLock();

	const int id = m_imageCapture->capture(outputFile());
	qDebug() << "Recording image to " << outputFile() << "with id" << id;

	// on shutter button released
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
#ifndef USE_QTMEL
	m_recorder->stop();
#else
	m_qtmelRecorder->stop();
	disconnect(m_cameraGrabber, &CameraGrabber::frameAvailable, this, &Recorder::showFrame);
#endif
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
#ifndef USE_QTMEL
	if (!m_camera->isCaptureModeSupported(captureMode))
	{
		QMessageBox::critical(this, tr("Camera Error"), tr("Capture mode %1 is not supported on this system.").arg(captureMode));

		return QDialog::Rejected;
	}
#endif

	// The result must not be Accepted!
	setResult(QDialog::Rejected);

#ifndef USE_QTMEL
	m_camera->setCaptureMode(captureMode);
	m_camera->start();

	m_cameraViewFinder->show();
#else
	m_cameraViewFinder->hide();
#endif
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
#ifndef USE_QTMEL
		waitUntilCameraIsReady();
#endif

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
		if (!waitForRecordedFile(m_mode))
		{
			QMessageBox::critical(this, tr("Error"), m_error);

			return QDialog::Rejected;
		}
	}

	stopAllRecording();
	// Stop the camera when everything is done.
#ifndef USE_QTMEL
	m_camera->stop();
#endif

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
		if (!waitForRecordedFile(m_mode))
		{
			QMessageBox::critical(this, tr("Error"), m_error);

			return QDialog::Rejected;
		}
	}

	stopAllRecording();

	return result;
}

Recorder::Recorder(QWidget *parent) : QDialog(parent), m_camera(nullptr), m_recorder(nullptr), m_cameraViewFinder(new QCameraViewfinder(this))
#ifdef USE_QTMEL
, m_cameraGrabber(new CameraGrabber(this))
, m_audioGrabber(new AudioGrabber(this))
, m_qtmelRecorder(new QtMELRecorder(this))
#endif
, m_finshedRecording(false), m_isRecording(false)
{
	setupUi(this);

	const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

	foreach (const QCameraInfo &cameraInfo, cameras)
	{
		if (m_camera == nullptr)
		{
			m_camera = new QCamera(cameraInfo);
		}

		qDebug() << "Info:" << cameraInfo << "Camera:" << m_camera;
	}

#ifdef USE_QTMEL
	// setup the camera grabber
	m_cameraGrabber->setLatency(65);
	m_qtmelRecorder->setImageGrabber(m_cameraGrabber);

	// setup the audio grabber
	AudioFormat format;
	format.setChannelCount(2);
	format.setSampleRate(44100);
	format.setFormat(AudioFormat::SignedInt16);

	m_audioGrabber->setDeviceIndex(0);
	m_audioGrabber->setFormat(format);
	m_qtmelRecorder->setAudioGrabber(m_audioGrabber);

	// x264 loseless fast preset
	VideoCodecSettings settings;
	settings.setCoderType(EncoderGlobal::Vlc);
	settings.setFlags(EncoderGlobal::LoopFilter);
	settings.setMotionEstimationComparison(1);
	settings.setPartitions(EncoderGlobal::I4x4 | EncoderGlobal::P8x8);
	settings.setMotionEstimationMethod(EncoderGlobal::Hex);
	settings.setSubpixelMotionEstimationQuality(3);
	settings.setMotionEstimationRange(16);
	settings.setGopSize(250);
	settings.setMinimumKeyframeInterval(25);
	settings.setSceneChangeThreshold(40);
	settings.setIQuantFactor(0.71f);
	settings.setBFrameStrategy(1);
	settings.setQuantizerCurveCompressionFactor(0.6f);
	settings.setMinimumQuantizer(0);
	settings.setMaximumQuantizer(69);
	settings.setMaximumQuantizerDifference(4);
	settings.setDirectMvPredictionMode(EncoderGlobal::SpatialMode);
	settings.setFlags2(EncoderGlobal::FastPSkip);
	settings.setConstantQuantizerMode(0);
	settings.setPFramePredictionAnalysisMethod(EncoderGlobal::NoWpm);

	m_qtmelRecorder->encoder()->setVideoCodecSettings(settings);

	AudioCodecSettings audioSettings;
	audioSettings.setSampleRate(m_audioGrabber->format().sampleRate());
	audioSettings.setChannelCount(m_audioGrabber->format().channelCount());
	audioSettings.setSampleFormat(EncoderGlobal::Signed16);

	m_qtmelRecorder->encoder()->setAudioCodecSettings(audioSettings);
	m_qtmelRecorder->encoder()->setVideoCodec(EncoderGlobal::H264);
	m_qtmelRecorder->encoder()->setAudioCodec(EncoderGlobal::MP3);
	m_qtmelRecorder->encoder()->setOutputPixelFormat(EncoderGlobal::YUV420P);

	QImage blackImage(640, 480, QImage::Format_RGB888);
	blackImage.fill(Qt::black);

	m_frameLabel = new QLabel(this);
	m_frameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_frameLabel->setPixmap(QPixmap::fromImage(blackImage));
	verticalLayout->insertWidget(0, m_frameLabel);
	verticalLayout->setAlignment(m_frameLabel, Qt::AlignCenter);
	m_frameLabel->repaint();

	connect(m_qtmelRecorder, &QtMELRecorder::stateChanged, this, &Recorder::videoRecorderStateChangedQtMEL);

	/*
	TODO fix syntax
	connect(m_qtmelRecorder->encoder(), &Encoder::error, this, &gustav::Recorder::onEncoderError);
	connect(m_cameraGrabber, &AbstractGrabber::error, this, &gustav::Recorder::onGrabberError);
	connect(m_audioGrabber, &AbstractGrabber::error, this, &gustav::Recorder::onGrabberError);
	*/
#endif

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

Recorder::~Recorder()
{
#ifdef USE_QTMEL
	delete m_frameLabel;
	m_frameLabel = nullptr;

	delete m_cameraGrabber;
	m_cameraGrabber = nullptr;

	delete m_audioGrabber;
	m_audioGrabber = nullptr;

	delete m_qtmelRecorder;
	m_qtmelRecorder = nullptr;
#endif
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

#ifdef USE_QTMEL
void Recorder::videoRecorderStateChangedQtMEL(QtMELRecorder::State state)
{
	qDebug() << "Changed video recorder changed" << state;

	switch (state)
	{
		case QtMELRecorder::StoppedState:
		{
			m_isRecording = false;
			m_finshedRecording = true;

			break;
		}

		case QtMELRecorder::SuspendedState:
		{
			m_isRecording = false;

			break;
		}

		case QtMELRecorder::ActiveState:
		{
			m_isRecording = true;

			break;
		}
	}
}

void Recorder::onEncoderError(Encoder::Error error)
{
	qDebug() << "Encoder's error number: " << error;
	qDebug() << "Encoder's error: " << m_qtmelRecorder->encoder()->errorString();
}

void Recorder::onGrabberError(AbstractGrabber::Error error)
{
	qDebug()<<"Grabber's error number: "<<error;
}
#endif

void Recorder::videoRecorderStateChanged(QMediaRecorder::State state)
{
	qDebug() << "Video recorder state changed in recorder" << state;

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

#ifdef USE_QTMEL
void Recorder::showFrame(const QImage &frame)
{
	m_frameLabel->setPixmap(QPixmap::fromImage(frame));
}
#endif

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

bool Recorder::waitForRecordedFile(Mode mode)
{
	QEventLoop eventLoop(this);

	while (!m_finshedRecording && (
		((mode == Mode::RecordVideo || mode == Mode::RecordAudio) && (
#ifndef USE_QTMEL
			m_recorder->error() == QMediaRecorder::NoError
#else
			/*
			 * If it is recording a video, check for the error states.
			 */
			(mode == Mode::RecordVideo &&
			(
				m_qtmelRecorder->encoder()->error() == Encoder::NoError
				&& m_qtmelRecorder->imageGrabber()->error() == AbstractGrabber::NoError
			))
			|| mode != Mode::RecordVideo
#endif
		)
		)
		|| mode == Mode::CaptureImage
	))
	{
		/*
		 * Check every second for the conditions to end the blocking.
		 * Handle all GUI events (stop buttons etc.) during the blocking.
		 */
		eventLoop.processEvents(QEventLoop::AllEvents, 1000);
	}

#ifndef USE_QTMEL
	qDebug() << "Recorded file is " << m_finshedRecording << "and error is " << m_recorder->error() << "and status is" << m_recorder->status();

	if (m_recorder->error() != QMediaRecorder::NoError)
	{
		m_error = m_recorder->error();

		return false;
	}
#else
	qDebug() << "Recorded file is " << m_finshedRecording << "and encoder error is " << m_qtmelRecorder->encoder()->error() << "and recorder error is" << m_qtmelRecorder->imageGrabber()->error() << "and state is" << m_qtmelRecorder->state();

	if (mode == Mode::RecordVideo && (
		m_qtmelRecorder->encoder()->error() != Encoder::NoError
		|| m_qtmelRecorder->imageGrabber()->error() != AbstractGrabber::NoError))
	{
		m_error = tr("Encoder error code %1: %2. Image grabber error code %3: %4.").arg(m_qtmelRecorder->encoder()->error()).arg(m_qtmelRecorder->encoder()->errorString()).arg(m_qtmelRecorder->imageGrabber()->error()).arg(m_qtmelRecorder->imageGrabber()->errorString());

		return false;
	}
#endif

	eventLoop.quit();

	m_finshedRecording = false;

	return true;
}

}

#include "moc_recorder.cpp"
