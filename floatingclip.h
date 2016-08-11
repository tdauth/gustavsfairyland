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
		FloatingClip(RoomWidget *parent, int width = 160, int speed = FLOATING_CLIP_PIXELS_PER_S);

		/**
		 * Paints the floating clip with \p painter on \p area.
		 */
		void paint(QPainter *painter, QWidget *area);

		/**
		 * Sets the current clip reference for the floating clip. The image of the clip will be shown on the floating clip.
		 * \param clip The referenced clip
		 */
		void setClip(Clip *clip);
		int speed() const;
		int width() const;
		int x() const;
		int y() const;

		void move(int x, int y);

		void start();
		void pause();
		void resume();

		bool contains(const QPoint &pos) const;

		void updatePosition(int intervalMs);

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
		Clip *m_clip;
		/// Store the scaled version of the clip's image to improve performance.
		QPixmap m_scaledPixmap;
};

inline void FloatingClip::setClip(Clip *clip)
{
	this->m_clip = clip;
	this->updateScaledClipImage();
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

inline void FloatingClip::move(int x, int y)
{
	this->m_x = x;
	this->m_y = y;
}

inline bool FloatingClip::contains(const QPoint &pos) const
{
	return QRect(x(), y(), m_width, m_width).contains(pos);
}

#endif // FLOATINGCLIP_H
