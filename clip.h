#ifndef CLIP_H
#define CLIP_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

class Clip : public QObject
{
	Q_OBJECT

	public:
		Clip(const QUrl &imageUrl, const QUrl &videoUrl, const QUrl &narratorVideoUrl, QObject *parent = 0);

		QUrl imageUrl() const;
		QUrl videoUrl() const;
		QUrl narratorVideoUrl() const;

	private:
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		QUrl m_narratorVideoUrl;
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


#endif // CLIP_H
