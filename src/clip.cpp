#include <QtCore/QLocale>

#include "clip.h"
#include "fairytale.h"


Clip::Clip(fairytale *app, QObject *parent): AbstractClip(app, parent), m_isPerson(false)
{
}

Clip::Clip(const Clip &clip, QObject *parent) : AbstractClip(clip, parent)
{
	this->assign(clip);
}

Clip::Clip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Urls &narratorUrls, const Descriptions &descriptions, bool isPerson, fairytale *app, QObject* parent): AbstractClip(id, imageUrl, videoUrl, descriptions, app, parent), m_narratorUrls(narratorUrls), m_isPerson(isPerson)
{
}

void Clip::assign(const AbstractClip &clip)
{
	AbstractClip::assign(clip);
	const Clip &source = dynamic_cast<const Clip&>(clip);
	
	this->m_narratorUrls = source.narratorUrls();
	this->m_isPerson = source.isPerson();
}

QUrl Clip::narratorUrl() const
{
	if (app() != nullptr)
	{
		const Urls::const_iterator iterator = this->m_narratorUrls.find(app()->currentTranslation());

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
