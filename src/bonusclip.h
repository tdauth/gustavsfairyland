#ifndef BONUSCLIP_H
#define BONUSCLIP_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QUrl>
#include <QLocale>

class fairytale;

/**
 * \brief A clip with additional content which has to be unlocked.
 */
class BonusClip : public QObject
{
	public:
		typedef QMap<QString,QString> Descriptions;

		BonusClip(fairytale *app, QObject *parent = nullptr);
		BonusClip(const BonusClip &clip, QObject *parent = nullptr);
		BonusClip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Descriptions &descriptions, fairytale *app, QObject *parent = nullptr);

		void assign(const BonusClip &clip);

		void setId(const QString &id);
		QString id() const;
		void setImageUrl(const QUrl &url);
		QUrl imageUrl() const;
		void setVideoUrl(const QUrl &url);
		QUrl videoUrl() const;
		void setDescriptions(const Descriptions &descriptions);
		QString description() const;

	private:
		fairytale *m_app;
		QString m_id;
		QUrl m_imageUrl;
		QUrl m_videoUrl;
		Descriptions m_descriptions;
};

inline void BonusClip::setId(const QString &id)
{
	this->m_id = id;
}

inline QString BonusClip::id() const
{
	return this->m_id;
}

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

inline void BonusClip::setDescriptions(const Descriptions &descriptions)
{
	this->m_descriptions = descriptions;
}

#endif // BONUSCLIP_H
