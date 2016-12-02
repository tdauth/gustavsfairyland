#ifndef DOOR_H
#define DOOR_H

#include <QObject>
#include <QPainter>
#include <QWidget>

class RoomWidget;

/**
 * \brief A door widget can be open or closed by wind from a specific direction.
 *
 * Every door has a specific location which is specified by the type \ref Door::Location.
 * Besides it has a state which can be checked via \ref isOpen().
 *
 * \note By now it is actually displayed as a window rather than a door but has the same effect.
 */
class Door : public QObject
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

		Door(RoomWidget *parent, Location location);

		void paint(QPainter *painter, QWidget *area);

		void open();
		void close();
		bool isOpen() const;

	private:
		RoomWidget *m_roomWidget;
		Location m_location;
		bool m_isOpen;
};

inline void Door::open()
{
	this->m_isOpen = true;
}

inline void Door::close()
{
	this->m_isOpen = false;
}

inline bool Door::isOpen() const
{
	return this->m_isOpen;
}

#endif // DOOR_H
