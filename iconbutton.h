#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QtWidgets/QPushButton>

class IconButton : public QPushButton
{
	Q_OBJECT

	public:
		explicit IconButton(QWidget *parent = 0);

		void setFile(const QString &file);
		QString file() const;

	private:
		void updateIcon();

		QString m_file;
		QPixmap m_pixmap; // cache the pixmap for faster resizing

	protected:
		virtual void resizeEvent(QResizeEvent *e) override;
};

inline void IconButton::setFile(const QString &file)
{
	this->m_file = file;
	this->m_pixmap = QPixmap(file);
	updateIcon();
}

inline QString IconButton::file() const
{
	return this->m_file;
}

#endif // ICONBUTTON_H
