#include <iostream>

#include <QtCore>

#include "customfairytale.h"

CustomFairytale::CustomFairytale(QObject *parent) : QObject(parent)
{
}

void CustomFairytale::load(const QSettings &settings)
{
	this->m_clipIds.clear();

	this->setPackageId(settings.value("packageId").toString());
	this->setName(settings.value("name").toString());
	this->setClipIds(settings.value("clipIds").toString().split(";"));
}

void CustomFairytale::save(QSettings &settings)
{
	settings.setValue("packageId", this->packageId());
	settings.setValue("name", this->name());
	settings.setValue("clipIds", clipIds().join(";"));
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
	result->setPackageId(values.takeFirst());
	result->setName(name);
	result->setClipIds(values);

	return result;
}


QString CustomFairytale::toString()
{
	return this->packageId() + ";" +  clipIds().join(";");
}

