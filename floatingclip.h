#ifndef FLOATINGCLIP_H
#define FLOATINGCLIP_H

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>

class RoomWidget;
class Clip;

class FloatingClip : public QObject
{
	Q_OBJECT

	public:
		FloatingClip(RoomWidget *parent, int width = 80);

		void paint(QPainter *painter, QWidget *area);

		void setClip(Clip *clip);
		int x() const;
		int y() const;

		void move(int x, int y);

		void start();
		void pause();
		void resume();

		bool contains(const QPoint &pos) const;

	private slots:
		void tick();

	private:
		RoomWidget *m_roomWidget;
		/// Width and height since it is a square.p
		int m_width;
		int m_x;
		int m_y;
		Clip *m_clip;
		/// Updates the position of the floating clip depending on the open windows in a room.
		QTimer *m_moveTimer;
};

inline void FloatingClip::setClip(Clip *clip)
{
	this->m_clip = clip;
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
