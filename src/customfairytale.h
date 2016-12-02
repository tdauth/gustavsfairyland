#ifndef CUSTOMFAIRYTALE_H
#define CUSTOMFAIRYTALE_H

#include <QObject>
#include <QSettings>
#include <QList>
#include <QStringList>

/**
 * \brief Allows the player to store the result of a game.
 */
class CustomFairytale : public QObject
{
	public:
		/**
		 * The key of every clip is represented by the ID of its clip package and the ID of the clip itself.
		 * This key can be used to uniquely identify bonus clips even if several clip packages are being used.
		 */
		typedef QPair<QString, QString> ClipKey;
		typedef QList<ClipKey> ClipIds;

		CustomFairytale(QObject *parent);

		void load(const QSettings &settings);
		void save(QSettings &settings);

		static CustomFairytale* fromString(const QString &value, const QString &name, QObject *parent);
		QString toString();

		void setName(const QString &name);
		QString name() const;
		void setClipIds(const ClipIds &clipIds);
		const ClipIds& clipIds() const;

	private:
		QString m_name;
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

inline void CustomFairytale::setClipIds(const CustomFairytale::ClipIds &clipIds)
{
	this->m_clipIds = clipIds;
}

inline const CustomFairytale::ClipIds& CustomFairytale::clipIds() const
{
	return this->m_clipIds;
}

#endif // CUSTOMFAIRYTALE_H
