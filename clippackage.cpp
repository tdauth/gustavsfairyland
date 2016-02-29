#include <iostream>
#include <cassert>

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QTemporaryFile>

#include <QtCore/QXmlStreamReader>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "clippackage.h"
#include "clip.h"

#include "clippackage.moc"

ClipPackage::ClipPackage(QObject *parent) : QObject(parent)
{
}

bool ClipPackage::loadClipsFromArchive(const QString &file)
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
		return false;
	}

	// access blocks by their file identifier
	typedef QMultiMap<QString, Block> Blocks;
	Blocks blocks;

	std::cerr << "Blocks size " << header.blocks << std::endl;

	for (uint64_t i = 0; i < header.blocks; ++i)
	{
		Block block;
		f.read((char*)&block, sizeof(block));
		const QString name(block.name);
		blocks.insert(name, block);
	}

	QString clipsFilePath;

	// write all blocks into temporary files with file paths
	for (Blocks::iterator iterator = blocks.begin(); iterator != blocks.end(); ++iterator)
	{
		QTemporaryFile file;
		file.setFileTemplate(iterator.key());
		std::cerr << "Loading file " << iterator.key().toUtf8().constData() << std::endl;
		file.setAutoRemove(false);

		if (!file.open())
		{
			return false;
		}

		std::cerr << "Writing it into the temporary file: " << file.fileName().toUtf8().constData() << std::endl;

		if (!f.seek(iterator.value().offset))
		{
			return false;
		}

		const uint64_t dataSize = iterator.value().size;
		std::cerr << "Reading " << dataSize << " bytes " << std::endl;
		// TODO create on heap? if stack is too small?
		char data[dataSize];
		f.read(data, dataSize);

		if (file.write(data, dataSize) == -1)
		{
			return false;
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
	return loadClipsFromFile(clipsFilePath);
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
		return false;
	}

	/*
	 * The block table starts directly after the header.
	 */
	const qint64 blockTableStart = f.pos();

	qint64 blockTableOffset = blockTableStart;
	// +1 for the block of the clips file
	// if there is less blocks some bytes will stay unused
	qint64 offset = (this->clips().size() + 1) * sizeof(Block);

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
			const QString filePath = clip->imageUrl().toString();

			if (!writeBlock(filePath, f, imageBlock, offset, filePath, blocks))
			{
				return false;
			}
		}

		// write narrator file
		Block narratorBlock;
		const bool narratorBlockExists = !clip->narratorVideoUrl().isEmpty();

		if (narratorBlockExists)
		{
			const QString filePath = clip->narratorVideoUrl().toString();

			if (!writeBlock(filePath, f, narratorBlock, offset, filePath, blocks))
			{
				return false;
			}
		}

		// write video file
		Block videoBlock;
		const bool videoBlockExists = !clip->videoUrl().isEmpty();

		if (videoBlockExists)
		{
			const QString filePath = clip->videoUrl().toString();

			if (!writeBlock(filePath, f, videoBlock, offset, filePath, blocks))
			{
				return false;
			}
		}

		if (!f.seek(blockTableOffset))
		{
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

	std::cerr << "Clips tmp file: " << clipsFile.fileName().toUtf8().constData() << std::endl;

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

	std::cerr << "Size of clips block " << sizeof(clipsBlock) << std::endl;

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

	if (!imageFile.open(QIODevice::ReadOnly))
	{
		return false;
	}

	const QByteArray imageFileData = imageFile.readAll();
	out.write(imageFileData);
	block.offset = offset;
	block.size = out.pos() - offset;
	std::cerr << "Block size " << block.size << std::endl;
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

	std::cerr << "Block name: " << block.name << std::endl;

	offset += block.size;
	blocksCounter += 1;

	return true;
}
