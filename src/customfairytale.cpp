#include <iostream>

#include <QtCore>

#include "customfairytale.h"

CustomFairytale::CustomFairytale(QObject *parent) : QObject(parent)
{
}

void CustomFairytale::load(const QSettings &settings)
{
	this->m_clipIds.clear();

	this->setName(settings.value("name").toString());
	const QStringList clipIdPairs = settings.value("clipIds").toString().split(";");
	ClipIds clipIds;

	foreach (QString clipIdPair, clipIdPairs)
	{
		const QStringList pair = clipIdPair.split(":");
		clipIds.push_back(fairytale::ClipKey(pair.front(), pair.back()));
	}

	this->setClipIds(clipIds);
}

void CustomFairytale::save(QSettings &settings)
{
	settings.setValue("name", this->name());
	QStringList clipIdPairs;

	foreach (fairytale::ClipKey clipKey, clipIds())
	{
		clipIdPairs.push_back(clipKey.first + ":" + clipKey.second);
	}

	settings.setValue("clipIds", clipIdPairs.join(";"));
}

CustomFairytale* CustomFairytale::fromString(const QString& value, const QString &name, QObject *parent)
{
	QStringList values = value.split(";");

	if (values.isEmpty())
	{
		std::cerr << "Is empty: " << value.toStdString() << std::endl;

		return nullptr;
	}

	CustomFairytale *result = new CustomFairytale(parent);
	result->setName(name);

	ClipIds clipIds;

	foreach (QString clipIdPair, values)
	{
		const QStringList pair = clipIdPair.split(":");
		clipIds.push_back(fairytale::ClipKey(pair.front(), pair.back()));
	}

	result->setClipIds(clipIds);

	return result;
}


QString CustomFairytale::toString()
{
	QStringList clipIdPairs;

	foreach (fairytale::ClipKey clipKey, clipIds())
	{
		clipIdPairs.push_back(clipKey.first + ":" + clipKey.second);
	}

	return clipIdPairs.join(";");
}

