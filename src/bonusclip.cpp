#include "bonusclip.h"
#include "fairytale.h"

BonusClip::BonusClip(fairytale *app, QObject *parent) : QObject(parent), m_app(app)
{
}

BonusClip::BonusClip(const BonusClip &clip, QObject *parent) : QObject(parent)
{
	this->assign(clip);
}

BonusClip::BonusClip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Descriptions &descriptions, fairytale *app, QObject *parent) : QObject(parent), m_app(app), m_id(id), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_descriptions(descriptions)
{
}

void BonusClip::assign(const BonusClip &clip)
{
	this->m_app = clip.m_app;
	this->m_id = clip.id();
	this->m_descriptions = clip.m_descriptions;
	this->m_imageUrl = clip.imageUrl();
	this->m_videoUrl = clip.videoUrl();
}

QString BonusClip::description() const
{
	const Descriptions::const_iterator iterator = this->m_descriptions.find(m_app->currentTranslation());

	if (iterator != this->m_descriptions.end())
	{
		return iterator.value();
	}
	else if (!this->m_descriptions.empty())
	{
		return this->m_descriptions.first();
	}

	return "";
}
