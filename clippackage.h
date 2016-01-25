#ifndef CLIPPACKAGE_H
#define CLIPPACKAGE_H

#include <QtCore/QObject>
#include <QtCore/QList>

class Clip;

class ClipPackage : public QObject
{
	Q_OBJECT

	public:
		typedef QList<Clip*> Clips;

		ClipPackage(QObject *parent);

		bool loadClipsFromFile(const QString &file);

		const QString& filePath() const;
		const QString& name() const;
		const Clips& clips() const;
	private:
		QString m_filePath;
		QString m_name;
		Clips m_clips;
};

inline const QString& ClipPackage::filePath() const
{
	return this->m_filePath;
}

inline const QString& ClipPackage::name() const
{
	return this->m_name;
}

inline const ClipPackage::Clips& ClipPackage::clips() const
{
	return this->m_clips;
}


#endif // CLIPPACKAGE_H
