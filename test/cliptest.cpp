#include <QtTest/QtTest>

#include "cliptest.h"
#include "../clippackage.h"

QTEST_MAIN(ClipTest);

void ClipTest::initTestCase()
{
    // Called before the first testfunction is executed
}

void ClipTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void ClipTest::init()
{
    // Called before each testfunction is executed
}

void ClipTest::cleanup()
{
    // Called after every testfunction
}

void ClipTest::testLoadXml()
{
	ClipPackage pkg;
	QVERIFY(pkg.loadClipsFromFile("clips.xml"));
	QCOMPARE(pkg.clips().size(), 9);
}

#include "cliptest.moc"
