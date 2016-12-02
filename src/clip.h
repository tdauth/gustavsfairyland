#ifndef CLIP_H
#define CLIP_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QString>

class fairytale;

/**
 * \brief A single video clip with narration and preview image.
 */
class Clip : public QObject
{
	Q_OBJECT

	public:
		typedef QMap<QString, QUrl> Urls;
		typedef QMap<QString, QString> Descriptions;

		Clip(fairytale *app, QObject *parent = nullptr);
		Clip(const Clip &clip, QObject *parent = nullptr);
		Clip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Urls &narratorUrls, const Descriptions &descriptions, bool isPerson, fairytale *app, QObject *parent = nullptr);

		void assign(const Clip &clip);

		void setId(const QString &id);
		QString id() const;
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
		fairytale *m_app;
		QString m_id;
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		Urls m_narratorUrls;
		Descriptions m_descriptions;
		bool m_isPerson;
};

inline void Clip::setId(const QString &id)
{
	this->m_id = id;
}

inline QString Clip::id() const
{
	return this->m_id;
}

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

inline const Clip::Urls& Clip::narratorUrls() const
{
	return this->m_narratorUrls;
}

inline void Clip::setDescriptions(const Descriptions &descriptions)
{
	this->m_descriptions = descriptions;
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
