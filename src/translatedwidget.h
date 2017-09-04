#ifndef TRANSLATEDWIDGET_H
#define TRANSLATEDWIDGET_H

#include <QDialog>

class fairytale;

/**
 * \brief Abstract class for widgets which have a retranslateUi() method and need to be retranslated when the language changes.
 *
 * Derive all your widgets using UI forms from this class to implement the retranslation automatically.
 *
 * Besides it updates the size for the widget and all child widgets when the widget is shown.
 */
class TranslatedWidget : public QDialog
{
	public:
		TranslatedWidget(fairytale *app, QWidget *parent);

		/**
		 * \return Returns the corresponding application of the widget.
		 */
		fairytale* app() const;

		/**
		 * Call the predefined method of the Qt designer generated C++ class.
		 */
		virtual void retranslateUi(QWidget *widget) = 0;

	protected:
		virtual void changeEvent(QEvent *event) override;
		virtual void showEvent(QShowEvent *event) override;

	private:
		fairytale *m_app;
};

inline fairytale* TranslatedWidget::app() const
{
	return this->m_app;
}

#endif // TRANSLATEDWIDGET_H