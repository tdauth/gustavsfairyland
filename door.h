#ifndef DOOR_H
#define DOOR_H

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

/**
 * \brief A door widget can be open or closed by wind from a specific direction.
 *
 * Every door has a specific location which is specified by the type \ref Door::Location.
 * Besides it has a state which can be checked via \ref isOpen().
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

		Door(QObject *parent, Location location);

		void paint(QPainter *painter, QWidget *area);

		void open();
		void close();
		bool isOpen() const;

	private:
		Location m_location;
		bool m_isOpen;
};

inline void Door::open()
{
	this->m_isOpen = true;
	//qDebug() << "Open: " << (int) m_location;
}

inline void Door::close()
{
	this->m_isOpen = false;
	//qDebug() << "Close: " << (int) m_location;
}

inline bool Door::isOpen() const
{
	return this->m_isOpen;
}

#endif // DOOR_H
