#include <iostream>
#include <cassert>

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QTemporaryFile>

#include <QtCore/QXmlStreamReader>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/qdir.h>

#include "clippackage.h"
#include "clip.h"

#include "clippackage.moc"

ClipPackage::ClipPackage(QObject *parent) : QObject(parent)
{
}

ClipPackage::~ClipPackage()
{
	this->clear();
}

bool ClipPackage::loadClipsFromCompressedArchive(const QString &file, const QString &clipsDir)
{
	QFile f(file);

	if (!f.open(QIODevice::ReadOnly))
	{
		qDebug() << "Unable to open file" << file;

		return false;
	}

	QTemporaryFile tmpOut;
	tmpOut.setFileTemplate(f.fileName());

	if (!tmpOut.open())
	{
		return false;
	}

	const QByteArray fileContent = f.readAll();
	const QByteArray decompressedFileContent = qUncompress(fileContent);
	tmpOut.write(decompressedFileContent);
	tmpOut.flush(); // write everything into the file for reading, this line is important since we dont close the temporary file before reading from it
	f.close();

	return loadClipsFromArchive(tmpOut.fileName(), clipsDir);
}

bool ClipPackage::saveClipsToCompressedArchive(const QString &file)
{
	QFile f(file);

	if (!f.open(QIODevice::WriteOnly))
	{
		qDebug() << "Unable to open file" << file;

		return false;
	}

	QTemporaryFile tmpOut;
	tmpOut.setFileTemplate(f.fileName());

	if (!tmpOut.open())
	{
		qDebug() << "Unable to open temporary file" << tmpOut.fileName();

		return false;
	}

	if (!saveClipsToArchive(tmpOut.fileName()))
	{
		qDebug() << "Unable to save clips to the archive.";

		return false;
	}

	// dont buffer anything
	tmpOut.flush();
	const QByteArray fileContent = tmpOut.readAll();
	const QByteArray compressedFileContent = qCompress(fileContent, 9);

	return (f.write(compressedFileContent) != -1);
}


bool ClipPackage::loadClipsFromArchive(const QString &file, const QString &clipsDir)
{
	QFile f(file);

	if (!f.open(QIODevice::ReadOnly))
	{
		qDebug() << "Unable to open file" << file;

		return false;
	}

	ArchiveHeader header;
	f.read((char*)&header, sizeof(header));

	// at the moment only version 0 is supported
	if (header.version != 0)
	{
		qDebug() << "Unknown version:" << header.version;

		return false;
	}

	// access blocks by their file identifier
	typedef QMultiMap<QString, Block> Blocks;
	Blocks blocks;

	for (uint64_t i = 0; i < header.blocks; ++i)
	{
		Block block;
		f.read((char*)&block, sizeof(block));
		const QString name(block.name);
		blocks.insert(name, block);
	}

	// write all clip files into sub dir with the name of the package
	QDir dir(clipsDir);

	if (!dir.exists())
	{
		qDebug() << "Dir does not exist:" << dir;

		return false;
	}

	QFileInfo fileInfo(file);

	// TODO extract to specified output dir and not to basename. If the file has no extension it is even the same name as the file's name.
	if (!dir.mkdir(fileInfo.baseName()))
	{
		qDebug() << "Dir cannot be created:" << dir;

		return false;
	}

	dir = dir.filePath(fileInfo.baseName());

	if (!dir.exists())
	{
		qDebug() << "Dir does not exist:" << dir;

		return false;
	}

	QString clipsFilePath;

	// write all blocks into permanent files with file paths that they can be accessed
	for (Blocks::iterator iterator = blocks.begin(); iterator != blocks.end(); ++iterator)
	{
		QFile file(dir.filePath(iterator.key()));

		qDebug() << "Extracting file" << file.fileName();

		if (!file.open(QIODevice::WriteOnly))
		{
			return false;
		}

		const uint64_t offset = iterator.value().offset;

		if (!f.seek(offset))
		{
			return false;
		}

		const uint64_t dataSize = iterator.value().size;

		// if the block is empty don't read anything
		if (dataSize > 0)
		{
			// TODO create on heap? if stack is too small?
			char data[dataSize];
			f.read(data, dataSize);

			if (file.write(data, dataSize) == -1)
			{
				return false;
			}
		}

		if (iterator.key() == "clips.xml")
		{
			clipsFilePath = file.fileName();
		}
	}

	f.close();

	if (clipsFilePath.isEmpty())
	{
		std::cerr << "Missing clips file" << std::endl;

		return false;
	}

	// TODO remove temporary files sometime or use permanent files in the clips dir
	if (!loadClipsFromFile(clipsFilePath))
	{
		return false;
	}

	if (!this->removeDir())
	{
		return false;
	}

	m_dir = dir.absolutePath();

	return true;
}

bool ClipPackage::saveClipsToArchive(const QString &file)
{
	QFile f(file);

	if (!f.open(QIODevice::WriteOnly))
	{
		qDebug() << "Unable to open file" << file;

		return false;
	}

	const qint64 headerStart = 0;

	if (!f.seek(sizeof(ArchiveHeader)))
	{
		qDebug() << "Unable to skip the archive header";

		return false;
	}

	/*
	 * The block table starts directly after the header.
	 */
	const qint64 blockTableStart = f.pos();
	qint64 blockTableOffset = blockTableStart;

	// at most 3 blocks per clip
	// +1 for the block of the clips file
	// if there is less blocks some bytes will stay unused
	qint64 offset = blockTableStart + ((this->clips().size() * 3) + 1) * sizeof(Block);

	// skip the block table space and write it later when we have the information about the blocks
	if (!f.seek(offset))
	{
		return false;
	}

	uint64_t blocks = 0;

	for (int i = 0; i < this->clips().size(); ++i)
	{
		const Clip *clip = this->clips().at(i);

		// write image file
		Block imageBlock;
		const bool imageBlockExists = !clip->imageUrl().isEmpty();

		if (imageBlockExists)
		{
			const QString filePath = clip->imageUrl().toLocalFile();

			if (!writeBlock(filePath, f, imageBlock, offset, filePath, blocks))
			{
				qDebug() << "Unable to write the image block.";

				return false;
			}
		}

		// write narrator file
		Block narratorBlock;
		const bool narratorBlockExists = !clip->narratorVideoUrl().isEmpty();

		if (narratorBlockExists)
		{
			const QString filePath = clip->narratorVideoUrl().toLocalFile();

			if (!writeBlock(filePath, f, narratorBlock, offset, filePath, blocks))
			{
				qDebug() << "Unable to write the narrator block.";

				return false;
			}
		}

		// write video file
		Block videoBlock;
		const bool videoBlockExists = !clip->videoUrl().isEmpty();

		if (videoBlockExists)
		{
			const QString filePath = clip->videoUrl().toLocalFile();

			if (!writeBlock(filePath, f, videoBlock, offset, filePath, blocks))
			{
				qDebug() << "Unable to write the video block.";

				return false;
			}
		}

		if (!f.seek(blockTableOffset))
		{
			qDebug() << "Unable to seek to the block table.";

			return false;
		}

		if (imageBlockExists)
		{
			if (f.write((const char*)&imageBlock, sizeof(imageBlock)) == -1)
			{
				return false;
			}
		}

		if (narratorBlockExists)
		{
			if (f.write((const char*)&narratorBlock, sizeof(narratorBlock)) == -1)
			{
				return false;
			}
		}

		if (videoBlockExists)
		{
			if (f.write((const char*)&videoBlock, sizeof(videoBlock)) == -1)
			{
				return false;
			}
		}

		// store current block table offset for later use
		blockTableOffset = f.pos();

		// jump back to the data stream
		if (!f.seek(offset))
		{
			return false;
		}
	}

	/*
	 * The file "clips.xml" contains a list of all clip data which is contained in the archive.
	 */
	QTemporaryFile clipsFile;

	// open to generate file name
	if (!clipsFile.open())
	{
		return false;
	}

	if (!saveClipsToFile(clipsFile.fileName()))
	{
		return false;
	}

	Block clipsBlock;

	if (!writeBlock(clipsFile.fileName(), f, clipsBlock, offset, "clips.xml", blocks))
	{
		return false;
	}

	if (!f.seek(blockTableOffset))
	{
		return false;
	}

	if (f.write((const char*)&clipsBlock, sizeof(clipsBlock)) == -1)
	{
		return false;
	}

	blockTableOffset = f.pos();

	ArchiveHeader header;
	header.version = 0;
	header.blocks = blocks;

	if (!f.seek(headerStart))
	{
		return false;
	}

	if (f.write((const char*)&header, sizeof(header)) == -1)
	{
		return false;
	}

	if (!f.seek(offset))
	{
		return false;
	}

	return true;
}

bool ClipPackage::loadClipsFromFile(const QString &file)
{
	QFile f(file);

	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Unable to open file" << file;

		return false;
	}

	QDomDocument document;

	if (!document.setContent(&f))
	{
		std::cerr << "Error on reading DOM tree" << std::endl;

		return false;
	}

	std::cerr << document.nodeName().toUtf8().constData() << std::endl;

	const QDomElement root = document.firstChildElement();

	if (root.nodeName() != "clips")
	{
		std::cerr << "Missing <clips>" << std::endl;

		return false;
	}

	this->m_name = root.attribute("name");

	QDomNodeList nodes = root.elementsByTagName("clip");

	if (nodes.isEmpty())
	{
		std::cerr << "Missing clips" << std::endl;

		return false;
	}

	for (int i = 0; i < nodes.size(); ++i)
	{
		QDomNode node = nodes.at(i);

		if (node.nodeName() != "clip")
		{
			std::cerr << "Missing clip" << std::endl;

			return false;
		}

		const QUrl image = QUrl(node.firstChildElement("image").text());
		const QUrl video = QUrl(node.firstChildElement("video").text());
		const QUrl narrator = QUrl(node.firstChildElement("narrator").text());
		const QString description = node.firstChildElement("description").text();

		const QString isPerson = node.hasAttributes() && node.attributes().contains("isPerson") ?  node.attributes().namedItem("isPerson").nodeValue() : "";

		m_clips.push_back(new Clip(image, video, narrator, description, isPerson == "true", this));
	}

	std::cerr << "Clips " << m_clips.size() << std::endl;

	this->m_filePath = file;

	return true;
}

bool ClipPackage::saveClipsToFile(const QString& file)
{
	QDomDocument document;
	QDomElement root = document.createElement("clips");
	root.setAttribute("name", this->name());
	document.appendChild(root);

	for (int i = 0; i < this->clips().size(); ++i)
	{
		const Clip *clip = this->clips().at(i);
		QDomElement clipElement = document.createElement("clip");
		clipElement.setAttribute("isPerson", (int)clip->isPerson());

		if (!clip->imageUrl().isEmpty())
		{
			QDomElement imageElement = document.createElement("image");
			clipElement.appendChild(imageElement);

			QDomText imageTextNode = document.createTextNode(clip->imageUrl().toString());
			imageElement.appendChild(imageTextNode);
		}

		if (!clip->narratorVideoUrl().isEmpty())
		{
			QDomElement narratorElement = document.createElement("narrator");
			clipElement.appendChild(narratorElement);

			QDomText narratorTextNode = document.createTextNode(clip->narratorVideoUrl().toString());
			narratorElement.appendChild(narratorTextNode);
		}

		if (!clip->videoUrl().isEmpty())
		{
			QDomElement videoElement = document.createElement("video");
			clipElement.appendChild(videoElement);

			QDomText videoTextNode = document.createTextNode(clip->videoUrl().toString());
			videoElement.appendChild(videoTextNode);
		}

		if (!clip->description().isEmpty())
		{
			QDomElement descriptionElement = document.createElement("description");
			clipElement.appendChild(descriptionElement);

			QDomText descriptionTextNode = document.createTextNode(clip->description());
			descriptionElement.appendChild(descriptionTextNode);
		}

		root.appendChild(clipElement);

	}

	QFile f(file);

	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Unable to open file" << file;

		return false;
	}

	f.write(document.toByteArray());

	return true;
}

bool ClipPackage::writeBlock(const QString& filePath, QFile& out, Block &block, qint64 &offset, const QString &blockFileName, uint64_t &blocksCounter)
{
	// write image file
	QFile imageFile(filePath);

	if (!imageFile.exists())
	{
		return false;
	}

	if (!imageFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "Cant open file " << filePath;

		return false;
	}

	assert(out.pos() == offset);

	const QByteArray imageFileData = imageFile.readAll();
	out.write(imageFileData);
	block.offset = offset;
	block.size = out.pos() - offset;
	// TODO limit block name to 256!
	memset(block.name, 0, 256);
	const std::size_t filePathSize = std::min((std::size_t)255, strlen(blockFileName.toUtf8().constData()) + 1);
	memcpy(block.name, blockFileName.toUtf8().constData(), filePathSize);

	// make sure the string is always NULL terminating
	if (block.name[filePathSize] != 0)
	{
		block.name[filePathSize] = 0;
	}

	assert(block.name[filePathSize] == 0);

	offset += block.size;
	blocksCounter += 1;

	return true;
}

bool ClipPackage::removeDir()
{
	if (!this->m_dir.isEmpty())
	{
		QDir dir(this->m_dir);

		if (dir.exists())
		{
			return dir.removeRecursively();
		}
	}

	return true;
}
