#ifndef CLIP_H
#define CLIP_H

#include "abstractclip.h"

/**
 * \brief A single video clip with narration and preview image.
 */
class Clip : public AbstractClip
{
	public:
		typedef QMap<QString, QUrl> Urls;

		Clip(fairytale *app, QObject *parent = nullptr);
		Clip(const Clip &clip, QObject *parent = nullptr);
		Clip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Urls &narratorUrls, const Descriptions &descriptions, bool isPerson, fairytale *app, QObject *parent = nullptr);

		virtual void assign(const AbstractClip &clip) override;

		/**
		 * Narrator URLs are used for the sound files which narrate a clip.
		 * One file for every locale.
		 * \{
		 */
		void setNarratorUrls(const Urls &urls);
		/// \return Returns the URL for the language of the current locale or the first existing.
		QUrl narratorUrl() const;
		/**
		 * \}
		 */
		const Urls& narratorUrls() const;
		void setIsPerson(bool isPerson);
		bool isPerson() const;
		bool isAct() const;

	private:
		Urls m_narratorUrls;
		bool m_isPerson;
};

inline void Clip::setNarratorUrls(const Urls& urls)
{
	this->m_narratorUrls = urls;
}

inline const Clip::Urls& Clip::narratorUrls() const
{
	return this->m_narratorUrls;
}

inline void Clip::setIsPerson(bool isPerson)
{
	this->m_isPerson = isPerson;
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
