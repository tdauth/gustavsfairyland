#include "clip.h"

#include "clip.moc"


Clip::Clip(const QUrl& imageUrl, const QUrl& videoUrl, const QUrl& narratorVideoUrl, bool isPerson, QObject* parent): QObject(parent), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_narratorVideoUrl(narratorVideoUrl), m_isPerson(isPerson)
{

}