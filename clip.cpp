#include "clip.h"

#include "clip.moc"


Clip::Clip(QObject *parent): QObject(parent)
{
}

Clip::Clip(const Clip &clip, QObject* parent): QObject(parent)
{
	this->assign(clip);
}

Clip::Clip(const QUrl &imageUrl, const QUrl &videoUrl, const Urls &narratorUrls, const Descriptions &descriptions, bool isPerson, QObject* parent): QObject(parent), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_narratorUrls(narratorUrls), m_descriptions(descriptions), m_isPerson(isPerson)
{
}

void Clip::assign(const Clip &clip)
{
	this->m_imageUrl = clip.imageUrl();
	this->m_videoUrl = clip.videoUrl();
	this->m_narratorUrls = clip.m_narratorUrls;
	this->m_descriptions = clip.m_descriptions;
	this->m_isPerson = clip.isPerson();
}
