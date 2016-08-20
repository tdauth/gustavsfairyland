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

