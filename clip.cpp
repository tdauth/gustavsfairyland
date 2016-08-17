#include <QtCore/QLocale>

#include "clip.h"
#include "fairytale.h"

#include "clip.moc"


Clip::Clip(fairytale *app, QObject *parent): QObject(parent), m_app(app)
{
}

Clip::Clip(const Clip &clip, QObject* parent) : QObject(parent)
{
	this->assign(clip);
}

Clip::Clip(const QUrl &imageUrl, const QUrl &videoUrl, const Urls &narratorUrls, const Descriptions &descriptions, bool isPerson, fairytale *app, QObject* parent): QObject(parent), m_app(app), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_narratorUrls(narratorUrls), m_descriptions(descriptions), m_isPerson(isPerson)
{
}

void Clip::assign(const Clip &clip)
{
	this->m_app = clip.m_app;
	this->m_imageUrl = clip.imageUrl();
	this->m_videoUrl = clip.videoUrl();
	this->m_narratorUrls = clip.m_narratorUrls;
	this->m_descriptions = clip.m_descriptions;
	this->m_isPerson = clip.isPerson();
}

QUrl Clip::narratorUrl() const
{
	if (m_app != nullptr)
	{
		const Urls::const_iterator iterator = this->m_narratorUrls.find(m_app->currentTranslation());

		if (iterator != this->m_narratorUrls.end())
		{
			return iterator.value();
		}
	}

	if (!this->m_narratorUrls.empty())
	{
		return this->m_narratorUrls.first();
	}

	return QUrl();
}

QString Clip::description() const
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