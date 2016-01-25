#ifndef CLIP_H
#define CLIP_H

#include <qt5/QtCore/QObject>

class Clip : public QObject
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanupTestCase();

		void init();
		void cleanup();

};

#endif // CLIP_H
