#ifndef CLIP_H
#define CLIP_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QUrl>

/**
 * \brief A single video clip with narration and preview image.
 */
class Clip : public QObject
{
	Q_OBJECT

	public:
		Clip(QObject *parent = nullptr);
		Clip(const Clip &clip, QObject *parent = nullptr);
		Clip(const QUrl &imageUrl, const QUrl &videoUrl, const QUrl &narratorVideoUrl, const QString &description, bool isPerson, QObject *parent = nullptr);

		void assign(const Clip &clip);

		void setImageUrl(const QUrl &url);
		QUrl imageUrl() const;
		void setVideoUrl(const QUrl &url);
		QUrl videoUrl() const;
		void setNarratorVideoUrl(const QUrl &url);
		QUrl narratorVideoUrl() const;
		void setDescription(const QString &description);
		QString description() const;
		void setIsPerson(bool isPerson);
		bool isPerson() const;
		bool isAct() const;

	private:
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		QUrl m_narratorVideoUrl;
		QString m_description;
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

inline void Clip::setNarratorVideoUrl(const QUrl& url)
{
	this->m_narratorVideoUrl = url;
}

inline QUrl Clip::narratorVideoUrl() const
{
  return this->m_narratorVideoUrl;
}

inline void Clip::setDescription(const QString& description)
{
	this->m_description = description;
}

inline QString Clip::description() const
{
	return this->m_description;
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
