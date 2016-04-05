#ifndef DOOR_H
#define DOOR_H

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

class Door : public QObject
{
	public:
		enum class Location : int
		{
			North,
			South,
			West,
			East,
			MaxLocations
		};

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
	qDebug() << "Open: " << (int) m_location;
}

inline void Door::close()
{
	this->m_isOpen = false;
	qDebug() << "Close: " << (int) m_location;
}

inline bool Door::isOpen() const
{
	return this->m_isOpen;
}

#endif // DOOR_H
