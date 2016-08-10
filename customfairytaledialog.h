#ifndef CUSTOMFAIRYTALEDIALOG_H
#define CUSTOMFAIRYTALEDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_customfairytale.h"
#include "fairytale.h"

class fairytale;
class Clip;
class IconButton;

/**
 * \brief This dialog lists all clips of a custom fairytale in a row and allows playing the custom fairytale.
 *
 * Use \ref addClip() to add a new clip to the end of your custom fairytale.
 *
 * Use \ref clear() to clear the custom fairytale completely.
 */
class CustomFairytaleDialog : public QDialog, protected Ui::CustomFairytaleWidget
{
	Q_OBJECT

	public slots:
		void addClip(Clip *clip);

	public:
		CustomFairytaleDialog(fairytale *app, QWidget *parent);

		void clear();

	protected:
		virtual void showEvent(QShowEvent *event) override;

	private:
		typedef QList<Clip*> Clips;
		typedef QList<IconButton*> ClipButtons;

		fairytale *m_app;
		Clips m_clips;
		ClipButtons m_clipButtons;
};

#endif // CUSTOMFAIRYTALEDIALOG_H
