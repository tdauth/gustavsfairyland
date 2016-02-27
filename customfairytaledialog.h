#ifndef CUSTOMFAIRYTALEDIALOG_H
#define CUSTOMFAIRYTALEDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_customfairytale.h"
#include "fairytale.h"

class fairytale;
class Clip;

class CustomFairytaleDialog : public QDialog, protected Ui::CustomFairytaleWidget
{
	Q_OBJECT

	public slots:
		void addClip(Clip *clip);

	public:
		CustomFairytaleDialog(fairytale *app, QWidget *parent);

		void clear();

	protected:
		virtual void closeEvent(QCloseEvent *event) override;

	private:
		typedef QList<QAbstractButton*> ClipButtons;

		fairytale *m_app;
		ClipButtons m_clipButtons;
};

#endif // CUSTOMFAIRYTALEDIALOG_H
