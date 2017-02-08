#ifndef CLIPPACKAGE_H
#define CLIPPACKAGE_H

#include <cstdint>

#include <QObject>
#include <QMap>
#include <QList>
#include <QFile>
#include <QString>
#include <QVector>
#include <QUrl>

class Clip;
class BonusClip;
struct Block;
class fairytale;

/**
 * \brief Clip packages contain one or several clips and can be packaged into an archive file or simply stored as XML file.
 *
 * Packages of clips contain the clip information (meta data) as well as the clip file itself (data).
 * They can be compressed as well to reduce the file space.
 *
 * If a clip package is distributed as XML file only, the corresponding clip files have to be distributed manually and the URLs in the XML files have to be updated.
 * It is a good idea to use relative URLs starting from the clips directory of the game.
 */
class ClipPackage : public QObject
{
	public:
		/**
		 * A map of all names in all supported languages.
		 * The key is the locale of the language such as "de" or "en".
		 * The value is the name of the clip package in the corresponding language.
		 */
		typedef QMap<QString, QString> Names;
		/**
		 * \brief A map of clips which every package stores. They key is the unique ID of a clip.
		 */
		typedef QMap<QString, Clip*> Clips;
		/**
		 * \brief A map of bonus clips which every package stores. They key is the unique ID of a bonus clip.
		 */
		typedef QMap<QString, BonusClip*> BonusClips;
		/**
		 * \brief A vector of URLs for the outro videos.
		 * One entry per difficulty \ref fairytale::Difficulty. The difficulty can be used as index but make sure that the index is in bounds.
		 */
		typedef QVector<QUrl> Outros;

		ClipPackage(fairytale *app, QObject *parent = nullptr);
		virtual ~ClipPackage();

		/**
		 * Loads a compressed archive file which has to contain the meta data XML file and all the binary data of the clip files.
		 * The content extracted into \p clipsDir.
		 * \param file The compressed archive file which is loaded.
		 * \param clipsDir The directory where all data is extracted to.
		 * \return Returns true if the clips have been loaded successfully from the archive. Otherwise it returns false.
		 */
		bool loadClipsFromCompressedArchive(const QString &file, const QString &clipsDir);
		bool saveClipsToCompressedArchive(const QString &file);

		bool loadClipsFromArchive(const QString &file, const QString &clipsDir);
		bool saveClipsToArchive(const QString &file);

		/**
		 * Loads the clips specified in the XML file \p file.
		 * \param file An XML file which specifies all clips of the package.
		 * \return Returns true if the clips have been loaded successfully. Otherwise it returns false.
		 */
		bool loadClipsFromFile(const QString &file);
		bool saveClipsToFile(const QString &file);

		/**
		 * Uses the currently set file path.
		 */
		bool saveClipsToFile();

		/**
		 * Clears all clips from the package.
		 */
		void clear();

		void addClip(Clip *clip);

		const QString& filePath() const;
		void setId(const QString &id);
		QString id() const;
		void setNames(const Names &names);
		const Names& names() const;
		QString name() const;
		Clips& clips();
		const Clips& clips() const;
		const BonusClips& bonusClips() const;
		void setIntro(const QUrl &video);
		QUrl intro() const;

		enum class Outro
		{
			Easy = 0,
			Normal = 1,
			Hard = 2,
			Mahlerisch = 3,
			Lost = 4
		};

		/**
		 * Sets the outro video for the difficulty \p index.
		 */
		void setOutro(int index, const QUrl &video);
		const Outros& outros() const;

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

		fairytale *m_app;
		/// Directory with all extracted clip files
		QString m_dir;
		/// File path to the clips.xml file
		QString m_filePath;
		QString m_id;
		Names m_names;
		Clips m_clips;
		/// Additional clips which can be unlocked.
		BonusClips m_bonusClips;
		QUrl m_intro;
		Outros m_outros;
};

inline void ClipPackage::clear()
{
	this->m_clips.clear();
	this->m_bonusClips.clear();
	this->m_outros.clear();
	this->removeDir();
}

inline const QString& ClipPackage::filePath() const
{
	return this->m_filePath;
}

inline void ClipPackage::setId(const QString &id)
{
	this->m_id = id;
}

inline QString ClipPackage::id() const
{
	return this->m_id;
}

inline void ClipPackage::setNames(const Names &names)
{
	this->m_names = names;
}

inline const ClipPackage::Names& ClipPackage::names() const
{
	return this->m_names;
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

inline void ClipPackage::setIntro(const QUrl &video)
{
	this->m_intro = video;
}

inline QUrl ClipPackage::intro() const
{
	return this->m_intro;
}

inline void ClipPackage::setOutro(int index, const QUrl &video)
{
	if (this->m_outros.size() <= index)
	{
		this->m_outros.resize(index + 1);
	}

	this->m_outros[index] = video;
}

inline const ClipPackage::Outros& ClipPackage::outros() const
{
	return this->m_outros;
}

#endif // CLIPPACKAGE_H
