#include "clip.h"

#include "clip.moc"


Clip::Clip(QObject *parent): QObject(parent)
{
}

Clip::Clip(const Clip &clip, QObject* parent): QObject(parent)
{
	this->assign(clip);
}

Clip::Clip(const QUrl &imageUrl, const QUrl &videoUrl, const QUrl &narratorVideoUrl, const QString &description, bool isPerson, QObject* parent): QObject(parent), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_narratorVideoUrl(narratorVideoUrl), m_description(description), m_isPerson(isPerson)
{
}

void Clip::assign(const Clip &clip)
{
	this->m_imageUrl = clip.imageUrl();
	this->m_videoUrl = clip.videoUrl();
	this->m_narratorVideoUrl = clip.narratorVideoUrl();
	this->m_description = clip.description();
	this->m_isPerson = clip.isPerson();
}
