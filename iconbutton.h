#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <qt5/QtWidgets/QPushButton>

class IconButton : public QPushButton
{
	Q_OBJECT

	public:
		explicit IconButton(QWidget *parent = 0);
	private:

	protected:
		virtual void resizeEvent(QResizeEvent *e) override;
};

#endif // ICONBUTTON_H
