#include <QtTest/QtTest>

#include "cliptest.h"
#include "../clippackage.h"
#include "../clip.h"

QTEST_GUILESS_MAIN(ClipTest);

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

	QFile::remove("image.jpg");
	QFile::remove("video.mp4");
	QFile::remove("narrator.mp4");

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
	ClipPackage pkg(nullptr);
	QVERIFY(pkg.loadClipsFromFile("clips.xml"));
	QCOMPARE(pkg.clips().size(), 9);
	QCOMPARE(pkg.intro().toString(), QString("./mahler/intro3.mkv"));
	QCOMPARE(pkg.outros().size(), 4);
	QCOMPARE(pkg.outros().at(0).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(1).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(2).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.outros().at(3).toString(), QString("./mahler/end5.mkv"));
	QCOMPARE(pkg.bonusClips().size(), 1);
}

void ClipTest::testSaveLoadXml()
{
	ClipPackage pkg(nullptr);
	QVERIFY(pkg.loadClipsFromFile("clips.xml"));
	QCOMPARE(pkg.clips().size(), 9);
	QVERIFY(pkg.saveClipsToFile("clips2.xml"));
	pkg.clear();
	QVERIFY(pkg.loadClipsFromFile("clips2.xml"));
	QCOMPARE(pkg.clips().size(), 9);
}

void ClipTest::testSaveLoadArchive()
{
	ClipPackage pkg(nullptr);
	Clip *clip = new Clip(nullptr);
	pkg.addClip(clip);

	QVERIFY(pkg.saveClipsToArchive("clips.pkg"));

	pkg.saveClipsToFile("testit.xml");

	QVERIFY(QFile::exists("clips.pkg"));

	ClipPackage loaded(nullptr);
	QVERIFY(loaded.loadClipsFromArchive("clips.pkg", QDir::currentPath()));
	QCOMPARE(loaded.clips().size(), 1);
}

void ClipTest::testSaveLoadArchiveWithFiles()
{
	ClipPackage pkg(nullptr);
	Clip::Urls narratorUrls;
	narratorUrls.insert("en", QUrl::fromLocalFile("narrator.mp4"));
	Clip::Descriptions descriptions;
	descriptions.insert("en", "test");
	Clip *clip = new Clip("test", QUrl::fromLocalFile("image.jpg"), QUrl::fromLocalFile("video.mp4"), narratorUrls, descriptions, false, nullptr);
	pkg.addClip(clip);

	// it is not possible to save an archive without the corresponding referenced files of the clip
	QVERIFY(!pkg.saveClipsToArchive("clips.pkg"));

	// create empty files which will be packed into the package archive
	QFile imageFile("image.jpg");
	QFile videoFile("video.mp4");
	QFile narratorFile("narrator.mp4");

	const QByteArray testContent("test");

	QVERIFY(imageFile.open(QIODevice::WriteOnly));
	imageFile.write(testContent);
	imageFile.close();

	QVERIFY(videoFile.open(QIODevice::WriteOnly));
	videoFile.write(testContent);
	videoFile.close();

	QVERIFY(narratorFile.open(QIODevice::WriteOnly));
	narratorFile.write(testContent);
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

	ClipPackage loaded(nullptr);
	QVERIFY(loaded.loadClipsFromArchive("clips.pkg", QDir::currentPath()));
	QCOMPARE(loaded.clips().size(), 1);
	clip = loaded.clips().first();

	// check if the loading extracted the corresponding files
	// since they all have the size 0 they wont exist
	const QDir extractDir = QDir("clips");

	QFile loadedImageFile(extractDir.filePath("image.jpg"));
	QVERIFY(loadedImageFile.open(QIODevice::ReadOnly));
	QByteArray content = loadedImageFile.readAll();
	QCOMPARE(content, testContent);

	QFile loadedVideoFile(extractDir.filePath("video.mp4"));
	QVERIFY(loadedVideoFile.open(QIODevice::ReadOnly));
	content = loadedVideoFile.readAll();
	QCOMPARE(content, testContent);

	QFile loadedNarratorFile(extractDir.filePath("narrator.mp4"));
	QVERIFY(loadedNarratorFile.open(QIODevice::ReadOnly));
	content = loadedNarratorFile.readAll();
	QCOMPARE(content, testContent);

	qDebug() << "Image URL: " << clip->imageUrl().toLocalFile();

	QCOMPARE(clip->imageUrl().toLocalFile().toUtf8().constData(), "image.jpg");
	QCOMPARE(clip->videoUrl().toLocalFile().toUtf8().constData(), "video.mp4");
	QCOMPARE(clip->narratorUrls().first().toLocalFile().toUtf8().constData(), "narrator.mp4");
	QCOMPARE(clip->descriptions().first().toUtf8().constData(), "test");
	QCOMPARE(clip->isPerson(), false);
}

void ClipTest::testSaveLoadCompressedArchive()
{
	ClipPackage pkg(nullptr);
	Clip *clip = new Clip(nullptr);
	pkg.addClip(clip);

	QVERIFY(pkg.saveClipsToCompressedArchive("clips.pkgc"));
	QVERIFY(QFile::exists("clips.pkgc"));

	ClipPackage loaded(nullptr);
	QVERIFY(loaded.loadClipsFromCompressedArchive("clips.pkgc", QDir::currentPath()));
	QCOMPARE(loaded.clips().size(), 1);
}

#include "cliptest.moc"
