#ifndef BONUSCLIP_H
#define BONUSCLIP_H

#include <QObject>
#include <QUrl>

/**
 * \brief A clip with additional content which has to be unlocked.
 */
class BonusClip : public QObject
{
	public:
		BonusClip(QObject *parent = nullptr);
		BonusClip(const BonusClip &clip, QObject *parent = nullptr);
		BonusClip(const QUrl &imageUrl, const QUrl &videoUrl, const QString &description, QObject *parent = nullptr);

		void assign(const BonusClip &clip);

		void setImageUrl(const QUrl &url);
		QUrl imageUrl() const;
		void setVideoUrl(const QUrl &url);
		QUrl videoUrl() const;
		void setDescription(const QString &description);
		QString description() const;

	private:
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		QString m_description;
};

inline void BonusClip::setImageUrl(const QUrl &url)
{
	this->m_imageUrl = url;
}

inline QUrl BonusClip::imageUrl() const
{
	return this->m_imageUrl;
}

inline void BonusClip::setVideoUrl(const QUrl &url)
{
	this->m_videoUrl = url;
}

inline QUrl BonusClip::videoUrl() const
{
	return this->m_videoUrl;
}

inline void BonusClip::setDescription(const QString &description)
{
	this->m_description = description;
}

inline QString BonusClip::description() const
{
	return this->m_description;
}

#endif // BONUSCLIP_H
