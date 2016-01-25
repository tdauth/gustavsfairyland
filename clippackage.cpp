#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QDebug>

#include <QtCore/QXmlStreamReader>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "clippackage.h"
#include "clip.h"

#include "clippackage.moc"

ClipPackage::ClipPackage(QObject *parent) : QObject(parent)
{
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
