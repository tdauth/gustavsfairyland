#include <QtTest/QtTest>

#include "recordtest.h"
#include "../recorder.h"

QTEST_MAIN(RecordTest);

void RecordTest::initTestCase()
{
	// Called before the first testfunction is executed
}

void RecordTest::cleanupTestCase()
{
	// Called after the last testfunction was executed
}

void RecordTest::init()
{
	// Called before each testfunction is executed
}

void RecordTest::cleanup()
{
	// Called after every testfunction
}

void RecordTest::testRecordVideo()
{
	Recorder recorder;
	recorder.setOutputFile("video");
	recorder.recordVideo();

	delay();

	qDebug() << "State:" << recorder.state();
	qDebug() << "Error:" << recorder.recorder()->error();
	qDebug() << "State:" << recorder.recorder()->errorString();
	recorder.stopRecordingVideo();

	delay();

	const QString filePath = recorder.recorder()->outputLocation().toLocalFile();
	QVERIFY(!filePath.isEmpty());
	const QFileInfo fileInfo(filePath);
	QVERIFY(fileInfo.exists());
}

void RecordTest::testCaptureImage()
{
	Recorder recorder;
	recorder.setOutputFile("image");
	recorder.captureImage();

	delay();

	qDebug() << "State:" << recorder.state();
	qDebug() << "Error:" << recorder.recorder()->error();
	qDebug() << "State:" << recorder.recorder()->errorString();

	delay();

	const QString filePath = "image.jpg"; // TODO get the exact file name by the slot
	QVERIFY(!filePath.isEmpty());
	const QFileInfo fileInfo(filePath);
	QVERIFY(fileInfo.exists());
}

void RecordTest::testRecordAudio()
{
	Recorder recorder;
	recorder.setOutputFile("audio");
	recorder.recordAudio();

	delay();

	qDebug() << "State:" << recorder.state();
	qDebug() << "Error:" << recorder.recorder()->error();
	qDebug() << "State:" << recorder.recorder()->errorString();
	recorder.stopRecordingAudio();

	delay();

	const QString filePath = recorder.audioRecorder()->outputLocation().toLocalFile();
	QVERIFY(!filePath.isEmpty());
	const QFileInfo fileInfo(filePath);
	QVERIFY(fileInfo.exists());
}

void RecordTest::delay()
{
	int i = 0;
	while (i < 10000000)
	{
		qApp->processEvents();
		++i;
	}
}

#include "recordtest.moc"
