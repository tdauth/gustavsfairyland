#ifndef FLOATINGCLIP_H
#define FLOATINGCLIP_H

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>

#include "speed.h"

class RoomWidget;
class Clip;

/**
 * \brief The floating clip which has to be catched in time. Otherwise the game is lost.
 *
 * The floating clip moves around in the room widget (\ref RoomWidget) depending on the current wind direction.
 */
class FloatingClip : public QObject
{
	Q_OBJECT

	public:
		FloatingClip(RoomWidget *parent, int width, int speed);

		/**
		 * Paints the floating clip with \p painter on \p area.
		 */
		void paint(QPainter *painter, QWidget *area);

		/**
		 * Sets the current clip reference for the floating clip. The image of the clip will be shown on the floating clip.
		 * \param clip The referenced clip
		 */
		void setClip(Clip *clip);
		void setSpeed(int speed);
		/**
		 * \return Returns the pixels per S the floating clip is moved.
		 */
		int speed() const;
		void setWidth(int width);
		int width() const;
		int x() const;
		int y() const;
		void setDirX(int dirX);
		int dirX() const;
		void setDirY(int dirY);
		int dirY() const;

		void move(int x, int y);

		void start();
		void pause();
		void resume();

		/**
		 * Calculates if the position is inside the scaled paper.
		 * \param pos The position which is checked.
		 * \return Returns true if the position is inside of the scaled paper. Otherwise it returns false.
		 */
		bool contains(const QPoint &pos) const;

		void updatePosition(qint64 elapsedTime);

	private:
		/**
		 * Scales down the clip image from the current clip. This should not be done too often since
		 * the performance is weak.
		 */
		void updateScaledClipImage();

		RoomWidget *m_roomWidget;
		int m_speed; /// Pixels per S
		/// Width and height since it is a square.p
		int m_width;
		int m_x;
		int m_y;
		int m_dirX;
		int m_dirY;
		int m_collisionDistance; // The distance which has been moved due to a collision. Every update after the collision will increase this counter until it reaches a maximum, then it will be reset. Every collision sets this to 1 again.
		Clip *m_clip;
		/// Store the scaled version of the clip's image to improve performance.
		QImage m_scaledImage;
		QImage m_scaledImageDisabled;
		QImage m_scaledImagePaper;
		QImage m_scaledImagePaperDisabled;
};

inline void FloatingClip::setClip(Clip *clip)
{
	this->m_clip = clip;
	this->updateScaledClipImage();
}


inline void FloatingClip::setSpeed(int speed)
{
	this->m_speed = speed;
}

inline int FloatingClip::speed() const
{
	return this->m_speed;
}

inline int FloatingClip::width() const
{
	return this->m_width;
}

inline int FloatingClip::x() const
{
	return this->m_x;
}

inline int FloatingClip::y() const
{
	return this->m_y;
}

inline void FloatingClip::setDirX(int dirX)
{
	this->m_dirX = dirX;
}

inline int FloatingClip::dirX() const
{
	return this->m_dirX;
}

inline void FloatingClip::setDirY(int dirY)
{
	this->m_dirY = dirY;
}

inline int FloatingClip::dirY() const
{
	return this->m_dirY;
}

inline void FloatingClip::move(int x, int y)
{
	this->m_x = x;
	this->m_y = y;
}

inline bool FloatingClip::contains(const QPoint &pos) const
{
	return QRect(x(), y(), m_scaledImagePaper.width(), m_scaledImagePaper.height()).contains(pos);
}

#endif // FLOATINGCLIP_H
