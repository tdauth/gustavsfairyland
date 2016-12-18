#ifndef BONUSCLIP_H
#define BONUSCLIP_H

#include "abstractclip.h"

/**
 * \brief A clip with additional content which has to be unlocked.
 */
class BonusClip : public AbstractClip
{
	public:
		BonusClip(fairytale *app, QObject *parent = nullptr);
		BonusClip(const BonusClip &clip, QObject *parent = nullptr);
		BonusClip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Descriptions &descriptions, fairytale *app, QObject *parent = nullptr);
};

#endif // BONUSCLIP_H
