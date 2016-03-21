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
	if (QFile::exists("clips.pkg"))
	{
		QFile::remove("clips.pkg");
	}

	if (QFile::exists("clips2.xml"))
	{
		QFile::remove("clips2.xml");
	}

	if (QFile::exists("clips.pkgc"))
	{
		QFile::remove("clips.pkgc");
	}

	QDir clipsDir("clips");

	if (clipsDir.exists())
	{
		clipsDir.removeRecursively();
	}
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

void ClipTest::testSaveLoadArchiveWithFiles()
{
	ClipPackage pkg;
	Clip *clip = new Clip(QUrl("image.jpg"), QUrl("video.mp4"), QUrl("narrator.mp4"), "test", false);
	pkg.addClip(clip);

	// it is not possible to save an archive without the corresponding referenced files of the clip
	QVERIFY(!pkg.saveClipsToArchive("clips.pkg"));

	// create empty files which will be packed into the package archive
	QFile imageFile("image.jpg");
	QFile videoFile("video.mp4");
	QFile narratorFile("narrator.mp4");

	QVERIFY(imageFile.open(QIODevice::WriteOnly));
	imageFile.close();

	QVERIFY(videoFile.open(QIODevice::WriteOnly));
	videoFile.close();

	QVERIFY(narratorFile.open(QIODevice::WriteOnly));
	narratorFile.close();

	QVERIFY(imageFile.exists());
	QVERIFY(videoFile.exists());
	QVERIFY(narratorFile.exists());

	QVERIFY(pkg.saveClipsToArchive("clips.pkg"));
	QVERIFY(QFile::exists("clips.pkg"));

	// now remove the files which should be extracted again on loading
	QVERIFY(imageFile.remove());
	QVERIFY(videoFile.remove());
	QVERIFY(narratorFile.remove());

	ClipPackage loaded;
	QVERIFY(loaded.loadClipsFromArchive("clips.pkg", QDir::currentPath()));
	QCOMPARE(loaded.clips().size(), 1);
	clip = loaded.clips().at(0);
	// check if the loading extracted the corresponding files
	// since they all have the size 0 they wont exist
	QVERIFY(!QFile::exists("image.jpg"));
	QVERIFY(!QFile::exists("video.mp4"));
	QVERIFY(!QFile::exists("narrator.mp4"));

	QCOMPARE(clip->imageUrl().toString().toUtf8().constData(), "image.jpg");
	QCOMPARE(clip->videoUrl().toString().toUtf8().constData(), "video.mp4");
	QCOMPARE(clip->narratorVideoUrl().toString().toUtf8().constData(), "narrator.mp4");
	QCOMPARE(clip->description().toUtf8().constData(), "test");
	QCOMPARE(clip->isPerson(), false);
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
