#ifndef RECORDERVIEW_H
#define RECORDERVIEW_H

#include <QDialog>
#include <QCameraViewfinder>
#include <QCamera>

#include "ui_recorderview.h"

class Recorder;

class RecorderView : public QDialog, protected Ui::RecorderView
{
	Q_OBJECT

	signals:
		void onPressPhotoButton();
		void onPressPauseButton();
		void onPressStopButton();

	private slots:
		void makePhoto();
		void recordVideo();
		void pauseRecording();
		void stopRecording();
	public:
		RecorderView(QWidget *parent, Recorder *recorder);

		void setCameraCaptureMode(QCamera::CaptureMode captureMode);

		QCameraViewfinder* cameraViewFinder() const;

	protected:
		virtual void hideEvent(QHideEvent *event) override;

	private:
		Recorder *m_recorder;
		QCameraViewfinder *m_cameraViewFinder;
};

inline QCameraViewfinder* RecorderView::cameraViewFinder() const
{
	return this->m_cameraViewFinder;
}

#endif // RECORDERVIEW_H
