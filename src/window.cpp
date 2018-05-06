#include <QtGui>

#include "window.h"
#include "roomwidget.h"

Window::Window(RoomWidget *parent, Location location): QObject(parent), m_roomWidget(parent), m_location(location), m_isOpen(false)
{
}