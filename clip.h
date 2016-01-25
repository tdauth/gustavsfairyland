#ifndef CLIP_H
#define CLIP_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QUrl>

class Clip : public QObject
{
	Q_OBJECT

	public:
		Clip(const QUrl &imageUrl, const QUrl &videoUrl, const QUrl &narratorVideoUrl, const QString &description, bool isPerson, QObject *parent = 0);

		QUrl imageUrl() const;
		QUrl videoUrl() const;
		QUrl narratorVideoUrl() const;
		QString description() const;
		bool isPerson() const;
		bool isAct() const;

	private:
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		QUrl m_narratorVideoUrl;
		QString m_description;
		bool m_isPerson;
};

inline QUrl Clip::imageUrl() const
{
  return this->m_imageUrl;
}

inline QUrl Clip::videoUrl() const
{
  return this->m_videoUrl;
}

inline QUrl Clip::narratorVideoUrl() const
{
  return this->m_narratorVideoUrl;
}

inline QString Clip::description() const
{
	return this->m_description;
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
