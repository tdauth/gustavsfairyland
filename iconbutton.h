#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QtWidgets/QPushButton>

/**
 * @brief A button which shows the icon for example of the image of a clip and resizes the icon automatically.
 *
 * Whenever the button is resized the shown image is resized automatically. This allows the image to grow with the button.
 */
class IconButton : public QPushButton
{
	public:
		explicit IconButton(QWidget *parent = 0);
		virtual ~IconButton();

		void setFile(const QString &file);
		QString file() const;

	private:
		/// Updates the icon of the button to the current image from the file scaled to the button's size.
		void updateIcon();

		QString m_file;
		QPixmap m_pixmap; // cache the pixmap for faster resizing

	protected:
		/// Updates the size of the shown image automatically.
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
