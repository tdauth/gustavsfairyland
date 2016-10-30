#include "bonusclip.h"

BonusClip::BonusClip(QObject *parent) : QObject(parent)
{
}

BonusClip::BonusClip(const BonusClip &clip, QObject *parent) : QObject(parent)
{
	this->assign(clip);
}

BonusClip::BonusClip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Descriptions &descriptions, QObject *parent) : QObject(parent), m_id(id), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_descriptions(descriptions)
{
}

void BonusClip::assign(const BonusClip &clip)
{
	this->m_id = clip.id();
	this->m_descriptions = clip.m_descriptions;
	this->m_imageUrl = clip.imageUrl();
	this->m_videoUrl = clip.videoUrl();
}
