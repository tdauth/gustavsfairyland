#include "bonusclip.h"

BonusClip::BonusClip(fairytale *app, QObject *parent) : AbstractClip(app, parent)
{
}

BonusClip::BonusClip(const BonusClip &clip, QObject *parent) : AbstractClip(clip, parent)
{
}

BonusClip::BonusClip(const QString &id, const QUrl &imageUrl, const QUrl &videoUrl, const Descriptions &descriptions, fairytale *app, QObject *parent) : AbstractClip(id, imageUrl, videoUrl, descriptions, app, parent)
{
}