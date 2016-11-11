#include <QtMultimedia>
#include <QtWidgets>

#include "recorderview.h"
#include "recorder.h"

RecorderView::RecorderView(QWidget *parent, Recorder *recorder) : QDialog(parent), m_recorder(recorder), m_cameraViewFinder(new QCameraViewfinder())
{
	setupUi(this);

	verticalLayout->addWidget(m_cameraViewFinder);

	connect(pausePushButton, &QPushButton::clicked, this, &RecorderView::pauseRecording);
	connect(stopPushButton, &QPushButton::clicked, this, &RecorderView::stopRecording);
	connect(cancelPushButton, &QPushButton::clicked, this, &RecorderView::reject);
}

void RecorderView::setCameraCaptureMode(QCamera::CaptureMode captureMode)
{
	if (captureMode == QCamera::CaptureStillImage)
	{
		photoPushButton->show();
		photoPushButton->setText(tr("Make Photo"));
		pausePushButton->hide();
		stopPushButton->hide();

		connect(photoPushButton, &QPushButton::clicked, this, &RecorderView::makePhoto);
	}
	else if (captureMode == QCamera::CaptureVideo)
	{
		photoPushButton->show();
		photoPushButton->setText(tr("Record Video"));
		pausePushButton->show();
		stopPushButton->show();

		connect(photoPushButton, &QPushButton::clicked, this, &RecorderView::recordVideo);
	}
}

void RecorderView::makePhoto()
{
	emit onPressPhotoButton();

	accept();
}

void RecorderView::recordVideo()
{
	emit onPressPhotoButton();
}

void RecorderView::pauseRecording()
{
	emit onPressPauseButton();
}

void RecorderView::stopRecording()
{
	emit onPressStopButton();

	accept();
}

void RecorderView::hideEvent(QHideEvent *hideEvent)
{
	qDebug() << "Stopping camera view";
	m_recorder->camera()->stop();
	QDialog::hideEvent(hideEvent);
}

#include "moc_recorderview.cpp"