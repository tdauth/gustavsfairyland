#ifndef ABSTRACTCLIP_H
#define ABSTRACTCLIP_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QMap>

class fairytale;

/**
 * \brief Abstract class for clips with image, video and some description.
 */
class AbstractClip : public QObject
{
	public:
		typedef QMap<QString, QString> Descriptions;

		AbstractClip(fairytale *app, QObject *parent = nullptr);
		AbstractClip(const AbstractClip &clip, QObject *parent = nullptr);
		AbstractClip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Descriptions &descriptions, fairytale *app, QObject *parent = nullptr);

		virtual void assign(const AbstractClip &clip);

		fairytale* app() const;

		void setId(const QString &id);
		QString id() const;
		void setImageUrl(const QUrl &url);
		QUrl imageUrl() const;
		void setVideoUrl(const QUrl &url);
		QUrl videoUrl() const;
		void setDescriptions(const Descriptions &description);
		/// \return Returns the description for the language of the current locale or the first existing.
		QString description() const;
		const Descriptions& descriptions() const;

	private:
		fairytale *m_app;
		QString m_id;
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		Descriptions m_descriptions;
};

inline fairytale* AbstractClip::app() const
{
	return this->m_app;
}

inline void AbstractClip::setId(const QString &id)
{
	this->m_id = id;
}

inline QString AbstractClip::id() const
{
	return this->m_id;
}

inline void AbstractClip::setImageUrl(const QUrl &url)
{
	this->m_imageUrl = url;
}

inline QUrl AbstractClip::imageUrl() const
{
	return this->m_imageUrl;
}

inline void AbstractClip::setVideoUrl(const QUrl& url)
{
	this->m_videoUrl = url;
}

inline QUrl AbstractClip::videoUrl() const
{
	return this->m_videoUrl;
}

inline void AbstractClip::setDescriptions(const Descriptions &descriptions)
{
	this->m_descriptions = descriptions;
}

inline const AbstractClip::Descriptions& AbstractClip::descriptions() const
{
	return this->m_descriptions;
}

#endif // ABSTRACTCLIP_H
