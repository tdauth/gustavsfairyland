#ifndef CLIP_H
#define CLIP_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QLocale>

/**
 * \brief A single video clip with narration and preview image.
 */
class Clip : public QObject
{
	Q_OBJECT

	public:
		typedef QMap<QString,QUrl> Urls;
		typedef QMap<QString,QString> Descriptions;

		Clip(QObject *parent = nullptr);
		Clip(const Clip &clip, QObject *parent = nullptr);
		Clip(const QUrl &imageUrl, const QUrl &videoUrl, const Urls &narratorUrls, const Descriptions &descriptions, bool isPerson, QObject *parent = nullptr);

		void assign(const Clip &clip);

		void setImageUrl(const QUrl &url);
		QUrl imageUrl() const;
		void setVideoUrl(const QUrl &url);
		QUrl videoUrl() const;
		void setNarratorUrls(const Urls &urls);
		/// \return Returns the URL for the language of the current locale or the first existing.
		QUrl narratorUrl() const;
		const Urls& narratorUrls() const;
		void setDescriptions(const Descriptions &description);
		/// \return Returns the description for the language of the current locale or the first existing.
		QString description() const;
		const Descriptions& descriptions() const;
		void setIsPerson(bool isPerson);
		bool isPerson() const;
		bool isAct() const;

	private:
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		Urls m_narratorUrls;
		Descriptions m_descriptions;
		bool m_isPerson;
};

inline void Clip::setImageUrl(const QUrl &url)
{
	this->m_imageUrl = url;
}

inline QUrl Clip::imageUrl() const
{
	return this->m_imageUrl;
}

inline void Clip::setVideoUrl(const QUrl& url)
{
	this->m_videoUrl = url;
}

inline QUrl Clip::videoUrl() const
{
	return this->m_videoUrl;
}

inline void Clip::setNarratorUrls(const Urls& urls)
{
	this->m_narratorUrls = urls;
}

inline QUrl Clip::narratorUrl() const
{
	QString locale = QLocale::system().name();
	locale.truncate(locale.lastIndexOf('_'));
	const Urls::const_iterator iterator = this->m_narratorUrls.find(locale);

	if (iterator != this->m_narratorUrls.end())
	{
		return iterator.value();
	}
	else if (!this->m_narratorUrls.empty())
	{
		this->m_narratorUrls.first();
	}

	return QUrl();
}

inline const Clip::Urls& Clip::narratorUrls() const
{
	return this->m_narratorUrls;
}

inline void Clip::setDescriptions(const Descriptions &descriptions)
{
	this->m_descriptions = descriptions;
}

inline QString Clip::description() const
{
	QString locale = QLocale::system().name();
	locale.truncate(locale.lastIndexOf('_'));
	const Descriptions::const_iterator iterator = this->m_descriptions.find(locale);

	if (iterator != this->m_descriptions.end())
	{
		return iterator.value();
	}
	else if (!this->m_descriptions.empty())
	{
		this->m_descriptions.first();
	}

	return "";
}

inline const Clip::Descriptions& Clip::descriptions() const
{
	return this->m_descriptions;
}

inline void Clip::setIsPerson(bool isPerson)
{
	this->m_isPerson = isPerson;
}

inline bool Clip::isPerson() const
{
	return this->m_isPerson;
}

inline bool Clip::isAct() const
{
	return !this->isPerson();
}

#endif // CLIP_H
