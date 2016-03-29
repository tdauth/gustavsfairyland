#ifndef ROOMWIDGET_H
#define ROOMWIDGET_H

#include <QtWidgets/QWidget>

/**
 * \brief A widget which displays a room with four windows from the top.
 *
 * Every window can open randomly and the wind can blow from that window and move the current piece of paper which contains the clip.
 * This makes it much harder for the player to select it without any hotkey.
 */
class RoomWidget : public QWidget
{
	public:
		RoomWidget(QWidget* parent, Qt::WindowFlags f);
		virtual QPaintEngine* paintEngine() const override;

	protected:
		virtual void paintEvent(QPaintEvent *event) override;
};

#endif // ROOMWIDGET_H
