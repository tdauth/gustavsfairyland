#ifndef TRANSLATEDWIDGET_H
#define TRANSLATEDWIDGET_H

#include <QWidget>

/**
 * \brief Abstract class for widgets which have a retranslateUi() method and need to be retranslated when the language changes.
 *
 * Derive all your widgets using UI forms from this class to implement the retranslation automatically.
 */
class TranslatedWidget : public QWidget
{
	public:
		TranslatedWidget();

		virtual void retranslateUi(QWidget *widget) = 0;

	protected:
		virtual void changeEvent(QEvent *event) override;
};

#endif // TRANSLATEDWIDGET_H
