#ifndef CLIP_H
#define CLIP_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

class Clip : public QObject
{
	Q_OBJECT

	public:
		Clip(const QUrl &imageUrl, const QUrl &videoUrl, const QUrl &narratorVideoUrl, bool isPerson, QObject *parent = 0);

		QUrl imageUrl() const;
		QUrl videoUrl() const;
		QUrl narratorVideoUrl() const;
		bool isPerson() const;
		bool isAct() const;

	private:
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		QUrl m_narratorVideoUrl;
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

inline bool Clip::isPerson() const
{
	return this->m_isPerson;
}

inline bool Clip::isAct() const
{
	return !this->isPerson();
}

#endif // CLIP_H
