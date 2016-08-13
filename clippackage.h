#ifndef CLIPPACKAGE_H
#define CLIPPACKAGE_H

#include <cstdint>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QFile>

class Clip;
class BonusClip;
struct Block;

/**
 * \brief Clip packages contain one or several clips and can be packaged into an archive file or simply stored as XML file.
 *
 * Packages of clips contain the clip information (meta data) as well as the clip file itself (data).
 * They can be compressed as well to reduce the file space.
 *
 * Besides they can be encrypted to prevent users from opening it.
 */
class ClipPackage : public QObject
{
	Q_OBJECT

	public:
		/**
		 * \brief A list of clips which every package stores.
		 */
		typedef QList<Clip*> Clips;
		typedef QList<BonusClip*> BonusClips;

		ClipPackage(QObject *parent = nullptr);
		virtual ~ClipPackage();

		bool loadClipsFromCompressedArchive(const QString &file, const QString &clipsDir);
		bool saveClipsToCompressedArchive(const QString &file);

		bool loadClipsFromArchive(const QString &file, const QString &clipsDir);
		bool saveClipsToArchive(const QString &file);

		bool loadClipsFromFile(const QString &file);
		bool saveClipsToFile(const QString &file);

		/**
		 * Clears all clips from the package.
		 */
		void clear();

		void addClip(Clip *clip);

		const QString& filePath() const;
		void setName(const QString &name);
		const QString& name() const;
		Clips& clips();
		const Clips& clips() const;
		const BonusClips& bonusClips() const;

		/**
		 * \return Returns the number of rounds possible with these clips.
		 */
		int rounds() const;

	private:
		/**
		 * Header structure for the archive format which is used for serialization of clip packages.
		 */
		struct ArchiveHeader
		{
			uint64_t version;
			/// The number of blocks stored in the archive. For each block one instanced of \ref Block is read.
			uint64_t blocks;
		};

		/**
		 * Block structure. Each block is stored with this structure in the block table.
		 */
		struct Block
		{
			/// The file name of the block data.
			char name[256];
			/// The absolute offset of the block in the package file.
			uint64_t offset;
			/// The size of the block starting from the block's offset.
			uint64_t size;
		};

		bool writeBlock(const QString &filePath, QFile &out, Block &block, qint64 &offset, const QString &blockFileName, uint64_t &blocksCounter);

		bool removeDir();

		/// Directory with all extracted clip files
		QString m_dir;
		/// File path to the clips.xml file
		QString m_filePath;
		QString m_name;
		Clips m_clips;
		/// Additional clips which can be unlocked.
		BonusClips m_bonusClips;
};

inline void ClipPackage::clear()
{
	this->m_clips.clear();
	this->m_bonusClips.clear();
	this->removeDir();
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

inline ClipPackage::Clips& ClipPackage::clips()
{
	return this->m_clips;
}

inline const ClipPackage::Clips& ClipPackage::clips() const
{
	return this->m_clips;
}

inline const ClipPackage::BonusClips& ClipPackage::bonusClips() const
{
	return this->m_bonusClips;
}

#endif // CLIPPACKAGE_H
