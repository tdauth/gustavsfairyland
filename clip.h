/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Barade <barade.barade@web.de>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef CLIP_H
#define CLIP_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

class Clip : public QObject
{
    Q_OBJECT
    
public:
  Clip(const QUrl &imageUrl, const QUrl &videoUrl, const QUrl &narratorVideoUrl, QObject *parent = 0);
  
  QUrl imageUrl() const;
  QUrl videoUrl() const;
  QUrl narratorVideoUrl() const;

private:
  QUrl m_imageUrl;
  QUrl m_videoUrl;
  QUrl m_narratorVideoUrl;
};

inline QUrl Clip::imageUrl() const
{
  return this->m_imageUrl;
}

inline QUrl Clip::videoUrl() const
{
  return this->m_videoUrl;
}

inline QUrl Clip::narratorVideoUrl() const
{
  return this->m_narratorVideoUrl;
}


#endif // CLIP_H
