#include "bonusclip.h"

BonusClip::BonusClip(QObject *parent) : QObject(parent)
{
}

BonusClip::BonusClip(const BonusClip &clip, QObject *parent) : QObject(parent)
{
	this->assign(clip);
}

BonusClip::BonusClip(const QUrl &imageUrl, const QUrl &videoUrl, const QString &description, QObject *parent) : QObject(parent), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_description(description)
{
}

void BonusClip::assign(const BonusClip &clip)
{
	this->m_description = clip.description();
	this->m_imageUrl = clip.imageUrl();
	this->m_videoUrl = clip.videoUrl();
}
