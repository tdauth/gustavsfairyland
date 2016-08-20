#ifndef CUSTOMFAIRYTALE_H
#define CUSTOMFAIRYTALE_H

#include <QObject>
#include <QSettings>
#include <QStringList>

/**
 * \brief Allows the player to store the result of a game.
 */
class CustomFairytale : public QObject
{
	public:
		typedef QStringList ClipIds;

		CustomFairytale(QObject *parent);

		void load(const QSettings &settings);
		void save(QSettings &settings);

		void setName(const QString &name);
		QString name() const;
		void setPackageId(const QString &packageId);
		QString packageId() const;
		void setClipIds(const ClipIds &clipIds);
		const ClipIds& clipIds() const;

	private:
		QString m_name;
		QString m_packageId;
		ClipIds m_clipIds;
};

inline void CustomFairytale::setName(const QString &name)
{
	this->m_name = name;
}

inline QString CustomFairytale::name() const
{
	return this->m_name;
}

inline void CustomFairytale::setPackageId(const QString &packageId)
{
	this->m_packageId = packageId;
}

inline QString CustomFairytale::packageId() const
{
	return this->m_packageId;
}

inline void CustomFairytale::setClipIds(const CustomFairytale::ClipIds &clipIds)
{
	this->m_clipIds = clipIds;
}

inline const CustomFairytale::ClipIds& CustomFairytale::clipIds() const
{
	return this->m_clipIds;
}

#endif // CUSTOMFAIRYTALE_H
