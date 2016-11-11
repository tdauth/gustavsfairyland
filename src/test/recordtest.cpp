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
	const QFileInfo fileInfo("tmp.mkv");
	recorder.setOutputFile(fileInfo.absoluteFilePath());
	recorder.recordVideo();
	qDebug() << "Recording to" << fileInfo.absoluteFilePath();
	//QThread::sleep(8);
	int i = 0;
	while (i < 10000000){
		qApp->processEvents();
		++i;
	}
	qDebug() << "State:" << recorder.state();
	qDebug() << "Error:" << recorder.recorder()->error();
	qDebug() << "State:" << recorder.recorder()->errorString();
	//QCOMPARE(recorder.state(), QMediaRecorder::RecordingState);
	recorder.stopRecordingVideo();
	QVERIFY(fileInfo.exists());

	/*
	QVERIFY(pkg.loadClipsFromFile("clips.xml"));
	QCOMPARE(pkg.clips().size(), 9);
	QCOMPARE(pkg.intro().toString(), QString("./mahler/intro3.mkv"));
	QCOMPARE(pkg.outros().size(), 4);
	QCOMPARE(pkg.outros().at(0).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(1).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(2).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(3).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.bonusClips().size(), 1);
	*/
}

void RecordTest::testCaptureImage()
{
	Recorder recorder;
	const QFileInfo fileInfo("tmp.jpg");
	recorder.setOutputFile(fileInfo.absoluteFilePath());
	recorder.recordImage();
	qDebug() << "Recording to" << fileInfo.absoluteFilePath();
	//QThread::sleep(8);
	int i = 0;
	while (i < 10000000){
		qApp->processEvents();
		++i;
	}
	qDebug() << "State:" << recorder.state();
	qDebug() << "Error:" << recorder.recorder()->error();
	qDebug() << "State:" << recorder.recorder()->errorString();
	//QCOMPARE(recorder.state(), QMediaRecorder::RecordingState);
	QVERIFY(fileInfo.exists());
}

void RecordTest::testRecordAudio()
{
	Recorder recorder;
	const QFileInfo fileInfo("tmp.wav");
	recorder.setOutputFile(fileInfo.absoluteFilePath());
	recorder.recordAudio();
	qDebug() << "Recording to" << fileInfo.absoluteFilePath();
	//QThread::sleep(8);
	int i = 0;
	while (i < 10000000){
		qApp->processEvents();
		++i;
	}
	qDebug() << "State:" << recorder.state();
	qDebug() << "Error:" << recorder.recorder()->error();
	qDebug() << "State:" << recorder.recorder()->errorString();
	//QCOMPARE(recorder.state(), QMediaRecorder::RecordingState);
	recorder.stopRecordingAudio();
	QVERIFY(fileInfo.exists());

	/*
	QVERIFY(pkg.loadClipsFromFile("clips.xml"));
	QCOMPARE(pkg.clips().size(), 9);
	QCOMPARE(pkg.intro().toString(), QString("./mahler/intro3.mkv"));
	QCOMPARE(pkg.outros().size(), 4);
	QCOMPARE(pkg.outros().at(0).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(1).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(2).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(3).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.bonusClips().size(), 1);
	*/
}

#include "recordtest.moc"
