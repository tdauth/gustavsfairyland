#ifndef CLIPTEST_H
#define CLIPTEST_H

#include <QtCore/QObject>

class ClipTest : public QObject
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanupTestCase();

		void init();
		void cleanup();

		void testLoadXml();

};

#endif // CLIPTEST_H
