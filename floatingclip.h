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
		FloatingClip(RoomWidget *parent, int width = 200, int speed = FLOATING_CLIP_PIXELS_PER_S);

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
		int speed() const;
		int width() const;
		int x() const;
		int y() const;

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

		QPair<int,int> updatePosition(qint64 elapsedTime);

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
