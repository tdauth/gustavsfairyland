#ifndef RECORDTEST_H
#define RECORDTEST_H

#include <QtCore/QObject>

class RecordTest : public QObject
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanupTestCase();

		void init();
		void cleanup();

		void testRecordVideo();
		void testCaptureImage();
		void testRecordAudio();

};

#endif // RECORDTEST_H
