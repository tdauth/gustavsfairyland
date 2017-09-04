#ifndef WINDOW_H
#define WINDOW_H

#include <QObject>
#include <QPainter>
#include <QWidget>

class RoomWidget;

/**
 * \brief A window widget can be open or closed by wind from a specific direction.
 *
 * Every window has a specific location which is specified by the type \ref Window::Location.
 * Besides it has a state which can be checked via \ref isOpen().
 */
class Window : public QObject
{
	public:
		enum class Location : int
		{
			North,
			South,
			West,
			East,
		};

		static const int MaxLocations = 4;

		Window(RoomWidget *parent, Location location);

		void paint(QPainter *painter, QWidget *area);

		void open();
		void close();
		bool isOpen() const;

	private:
		RoomWidget *m_roomWidget;
		Location m_location;
		bool m_isOpen;
};

inline void Window::open()
{
	this->m_isOpen = true;
}

inline void Window::close()
{
	this->m_isOpen = false;
}

inline bool Window::isOpen() const
{
	return this->m_isOpen;
}

#endif // WINDOW_H