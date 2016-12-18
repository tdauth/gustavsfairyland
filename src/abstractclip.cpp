#include <QtCore>

#include "abstractclip.h"
#include "fairytale.h"


AbstractClip::AbstractClip(fairytale *app, QObject *parent) : QObject(parent), m_app(app)
{
}

AbstractClip::AbstractClip(const AbstractClip &clip, QObject* parent) : QObject(parent)
{
	this->assign(clip);
}

AbstractClip::AbstractClip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Descriptions &descriptions, fairytale *app, QObject* parent): QObject(parent), m_app(app), m_id(id), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_descriptions(descriptions)
{
}

void AbstractClip::assign(const AbstractClip &clip)
{
	this->m_app = clip.m_app;
	this->m_id = clip.id();
	this->m_imageUrl = clip.imageUrl();
	this->m_videoUrl = clip.videoUrl();
	this->m_descriptions = clip.m_descriptions;
}

QString AbstractClip::description() const
{
	if (m_app != nullptr)
	{
		const Descriptions::const_iterator iterator = this->m_descriptions.find(m_app->currentTranslation());

		if (iterator != this->m_descriptions.end())
		{
			return iterator.value();
		}
	}

	if (!this->m_descriptions.empty())
	{
		return this->m_descriptions.first();
	}

	return "";
}
