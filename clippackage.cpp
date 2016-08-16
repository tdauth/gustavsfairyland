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
#include "bonusclip.h"

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
		std::cerr << "Unable to open file" << file.toStdString();

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
		std::cerr << "Unable to open file" << file.toStdString();

		return false;
	}

	QTemporaryFile tmpOut;
	tmpOut.setFileTemplate(f.fileName());

	if (!tmpOut.open())
	{
		std::cerr << "Unable to open temporary file" << tmpOut.fileName().toStdString();

		return false;
	}

	if (!saveClipsToArchive(tmpOut.fileName()))
	{
		std::cerr << "Unable to save clips to the archive.";

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
		std::cerr << "Unable to open file" << file.toStdString();

		return false;
	}

	ArchiveHeader header;
	f.read((char*)&header, sizeof(header));

	// at the moment only version 0 is supported
	if (header.version != 0)
	{
		std::cerr << "Unknown version:" << header.version;

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
		std::cerr << "Dir does not exist:" << dir.path().toStdString();

		return false;
	}

	const QFileInfo fileInfo(file);
	const QString subDirPath = fileInfo.baseName();

	if (!dir.mkdir(subDirPath))
	{
		std::cerr << "Dir cannot be created:" << dir.filePath(subDirPath).toStdString();

		return false;
	}

	dir = dir.filePath(fileInfo.baseName());

	if (!dir.exists())
	{
		std::cerr << "Dir does not exist:" << dir.path().toStdString();

		return false;
	}

	QString clipsFilePath;

	// write all blocks into permanent files with file paths that they can be accessed
	for (Blocks::iterator iterator = blocks.begin(); iterator != blocks.end(); ++iterator)
	{
		QFile file(dir.filePath(iterator.key()));

		std::cerr << "Extracting file" << file.fileName().toStdString();

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
		std::cerr << "Error on loading clips from file." << std::endl;

		return false;
	}

	if (!this->removeDir())
	{
		std::cerr << "Error on removing directory." << std::endl;

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
		std::cerr << "Unable to open file" << file.toStdString();

		return false;
	}

	const qint64 headerStart = 0;

	if (!f.seek(sizeof(ArchiveHeader)))
	{
		std::cerr << "Unable to skip the archive header";

		return false;
	}

	/*
	 * The block table starts directly after the header.
	 */
	const qint64 blockTableStart = f.pos();
	qint64 blockTableOffset = blockTableStart;

	qint64 countNarratorBlocks = 0;

	foreach (Clip *clip, this->clips())
	{
		countNarratorBlocks += clip->narratorUrls().size();
	}

	// at most 2 blocks per clip
	// + the number of narrator blocks
	// +1 for the block of the clips file
	// if there is less blocks some bytes will stay unused
	qint64 offset = blockTableStart + ((this->clips().size() * 2) + countNarratorBlocks + 1) * sizeof(Block);

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
				std::cerr << "Unable to write the image block.";

				return false;
			}
		}

		// write narrator files
		QList<Block> narratorBlocks;

		Clip::Urls::const_iterator iterator = clip->narratorUrls().constBegin();

		while (iterator != clip->narratorUrls().constEnd())
		{
			const QString filePath = iterator.value().toLocalFile();
			Block narratorBlock;

			if (!writeBlock(filePath, f, narratorBlock, offset, filePath, blocks))
			{
				std::cerr << "Unable to write the narrator block.";

				return false;
			}

			narratorBlocks.push_back(narratorBlock);

			++iterator;
		}

		// write video file
		Block videoBlock;
		const bool videoBlockExists = !clip->videoUrl().isEmpty();

		if (videoBlockExists)
		{
			const QString filePath = clip->videoUrl().toLocalFile();

			if (!writeBlock(filePath, f, videoBlock, offset, filePath, blocks))
			{
				std::cerr << "Unable to write the video block.";

				return false;
			}
		}

		if (!f.seek(blockTableOffset))
		{
			std::cerr << "Unable to seek to the block table.";

			return false;
		}

		if (imageBlockExists)
		{
			if (f.write((const char*)&imageBlock, sizeof(imageBlock)) == -1)
			{
				return false;
			}
		}

		// write all blocks for all written narrator files
		foreach (Block narratorBlock, narratorBlocks)
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

	for (int i = 0; i < this->bonusClips().size(); ++i)
	{
		const BonusClip *clip = this->bonusClips().at(i);

		// write image file
		Block imageBlock;
		const bool imageBlockExists = !clip->imageUrl().isEmpty();

		if (imageBlockExists)
		{
			const QString filePath = clip->imageUrl().toLocalFile();

			if (!writeBlock(filePath, f, imageBlock, offset, filePath, blocks))
			{
				std::cerr << "Unable to write the image block.";

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
				std::cerr << "Unable to write the video block.";

				return false;
			}
		}

		if (!f.seek(blockTableOffset))
		{
			std::cerr << "Unable to seek to the block table.";

			return false;
		}

		if (imageBlockExists)
		{
			if (f.write((const char*)&imageBlock, sizeof(imageBlock)) == -1)
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
		std::cerr << "Unable to open file" << file.toStdString();

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

	const QDomNodeList childs = root.childNodes();

	for (int j = 0; j < childs.size(); ++j)
	{
		const QDomNode parentNode = childs.at(j);
		const QDomElement parentElement = parentNode.toElement();

		if (parentElement.nodeName() == "bonusClip")
		{
			continue;
		}

		const bool isPerson = parentElement.nodeName() == "persons";
		const QDomNodeList nodes = parentElement.elementsByTagName("clip");

		for (int i = 0; i < nodes.size(); ++i)
		{
			const QDomNode node = nodes.at(i);

			if (node.nodeName() == "clip")
			{
				const QUrl image = QUrl(node.firstChildElement("image").text());
				const QUrl video = QUrl(node.firstChildElement("video").text());
				const QDomNodeList narratorNodes = node.firstChildElement("narrator").childNodes();
				const QDomNodeList descriptionNodes = node.firstChildElement("description").childNodes();

				Clip::Urls narratorUrls;

				for (int i = 0; i < narratorNodes.size(); ++i)
				{
					const QDomNode node = narratorNodes.at(i);
					narratorUrls.insert(node.nodeName(), QUrl(node.toElement().text()));
				}

				Clip::Descriptions descriptions;

				for (int i = 0; i < descriptionNodes.size(); ++i)
				{
					const QDomNode node = descriptionNodes.at(i);
					descriptions.insert(node.nodeName(), node.toElement().text());
				}

				m_clips.push_back(new Clip(image, video, narratorUrls, descriptions, isPerson, this));
			}
			else
			{
				std::cerr << "Unknow node:" << node.nodeName().toStdString();
			}
		}
	}

	const QDomNodeList nodes = root.elementsByTagName("bonusClip");

	for (int i = 0; i < nodes.size(); ++i)
	{
		QDomNode node = nodes.at(i);

		if (node.nodeName() == "bonusClip")
		{
			std::cerr << "Read bonus clip";

			const QUrl image = QUrl(node.firstChildElement("image").text());
			const QUrl video = QUrl(node.firstChildElement("video").text());
			const QDomNodeList descriptionNodes = node.firstChildElement("description").childNodes();

			Clip::Descriptions descriptions;

			for (int i = 0; i < descriptionNodes.size(); ++i)
			{
				const QDomNode node = descriptionNodes.at(i);
				descriptions.insert(node.nodeName(), node.toElement().text());
			}

			m_bonusClips.push_back(new BonusClip(image, video, descriptions, this));
		}
	}

	std::cerr << "Clips " << m_clips.size() << std::endl;
	std::cerr << "Bonus Clips Size " << m_bonusClips.size() << std::endl;

	this->m_filePath = file;

	return true;
}

bool ClipPackage::saveClipsToFile(const QString& file)
{
	QDomDocument document;
	QDomElement root = document.createElement("clips");
	root.setAttribute("name", this->name());
	document.appendChild(root);

	QDomElement persons = document.createElement("persons");
	root.appendChild(persons);
	QDomElement acts = document.createElement("acts");
	root.appendChild(acts);

	for (int i = 0; i < this->clips().size(); ++i)
	{
		const Clip *clip = this->clips().at(i);
		QDomElement clipElement = document.createElement("clip");

		if (!clip->imageUrl().isEmpty())
		{
			QDomElement imageElement = document.createElement("image");
			clipElement.appendChild(imageElement);

			QDomText imageTextNode = document.createTextNode(clip->imageUrl().toString());
			imageElement.appendChild(imageTextNode);
		}

		if (!clip->narratorUrls().isEmpty())
		{
			QDomElement narratorElement = document.createElement("narrator");
			clipElement.appendChild(narratorElement);

			Clip::Urls::const_iterator i = clip->narratorUrls().constBegin();

			while (i != clip->narratorUrls().constEnd())
			{
				QDomElement localeElement = document.createElement(i.key());
				narratorElement.appendChild(localeElement);

				QDomText narratorTextNode = document.createTextNode(i.value().toString());
				localeElement.appendChild(narratorTextNode);

				++i;
			}
		}

		if (!clip->videoUrl().isEmpty())
		{
			QDomElement videoElement = document.createElement("video");
			clipElement.appendChild(videoElement);

			QDomText videoTextNode = document.createTextNode(clip->videoUrl().toString());
			videoElement.appendChild(videoTextNode);
		}

		if (!clip->descriptions().isEmpty())
		{
			QDomElement descriptionElement = document.createElement("description");
			clipElement.appendChild(descriptionElement);


			Clip::Descriptions::const_iterator i = clip->descriptions().constBegin();

			while (i != clip->descriptions().constEnd())
			{
				QDomElement localeElement = document.createElement(i.key());
				descriptionElement.appendChild(localeElement);

				QDomText textNode = document.createTextNode(i.value());
				localeElement.appendChild(textNode);

				++i;
			}
		}

		if (clip->isPerson())
		{
			persons.appendChild(clipElement);
		}
		else
		{
			acts.appendChild(clipElement);
		}
	}

	for (int i = 0; i < this->bonusClips().size(); ++i)
	{
		const BonusClip *clip = this->bonusClips().at(i);
		QDomElement clipElement = document.createElement("bonusClip");

		if (!clip->imageUrl().isEmpty())
		{
			QDomElement imageElement = document.createElement("image");
			clipElement.appendChild(imageElement);

			QDomText imageTextNode = document.createTextNode(clip->imageUrl().toString());
			imageElement.appendChild(imageTextNode);
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
		std::cerr << "Unable to open file" << file.toStdString();

		return false;
	}

	f.write(document.toByteArray());

	return true;
}

int ClipPackage::rounds() const
{
	int persons = 0;
	int acts = 0;

	foreach (Clip *clip, m_clips)
	{
		if (clip->isPerson())
		{
			++persons;
		}
		else
		{
			++acts;
		}
	}

	if (acts > persons - 1)
	{
		return persons - 1;
	}

	return acts;
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
		std::cerr << "Cant open file " << filePath.toStdString();

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
