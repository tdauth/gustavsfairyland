#include "clip.h"

#include "clip.moc"


Clip::Clip(QObject *parent): QObject(parent)
{
}

Clip::Clip(const Clip& clip, QObject* parent): QObject(parent), m_imageUrl(clip.imageUrl()), m_videoUrl(clip.videoUrl()), m_narratorVideoUrl(clip.narratorVideoUrl()), m_description(clip.description()), m_isPerson(clip.isPerson())
{
}

Clip::Clip(const QUrl& imageUrl, const QUrl& videoUrl, const QUrl& narratorVideoUrl, const QString &description, bool isPerson, QObject* parent): QObject(parent), m_imageUrl(imageUrl), m_videoUrl(videoUrl), m_narratorVideoUrl(narratorVideoUrl), m_description(description), m_isPerson(isPerson)
{

}
