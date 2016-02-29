#include <QtTest/QtTest>

#include "cliptest.h"
#include "../clippackage.h"
#include "../clip.h"

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

void ClipTest::testSaveLoadXml()
{
	ClipPackage pkg;
	QVERIFY(pkg.loadClipsFromFile("clips.xml"));
	QCOMPARE(pkg.clips().size(), 9);
	QVERIFY(pkg.saveClipsToFile("clips2.xml"));
	pkg.clear();
	QVERIFY(pkg.loadClipsFromFile("clips2.xml"));
	QCOMPARE(pkg.clips().size(), 9);
}

void ClipTest::testSaveLoadArchive()
{
	ClipPackage pkg;
	Clip *clip = new Clip();
	pkg.addClip(clip);

	QVERIFY(pkg.saveClipsToArchive("clips.pkg"));
	QVERIFY(QFile::exists("clips.pkg"));

	ClipPackage loaded;
	QVERIFY(loaded.loadClipsFromArchive("clips.pkg", QDir::currentPath()));
	QCOMPARE(loaded.clips().size(), 1);
}

void ClipTest::testSaveLoadCompressedArchive()
{
	ClipPackage pkg;
	Clip *clip = new Clip();
	pkg.addClip(clip);

	QVERIFY(pkg.saveClipsToCompressedArchive("clips.pkgc"));
	QVERIFY(QFile::exists("clips.pkgc"));

	ClipPackage loaded;
	QVERIFY(loaded.loadClipsFromCompressedArchive("clips.pkgc", QDir::currentPath()));
	QCOMPARE(loaded.clips().size(), 1);
}

#include "cliptest.moc"
