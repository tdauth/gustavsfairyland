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
		bool saveClipsToFile(const QString &file);

		void clear();

		void addClip(Clip *clip);

		const QString& filePath() const;
		void setName(const QString &name);
		const QString& name() const;
		const Clips& clips() const;
	private:
		QString m_filePath;
		QString m_name;
		Clips m_clips;
};

inline void ClipPackage::clear()
{
	this->m_clips.clear();
}

inline void ClipPackage::addClip(Clip* clip)
{
	this->m_clips.push_back(clip);
}

inline const QString& ClipPackage::filePath() const
{
	return this->m_filePath;
}

inline void ClipPackage::setName(const QString& name)
{
	this->m_name = name;
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
