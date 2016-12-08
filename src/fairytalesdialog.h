#ifndef FAIRYTALESDIALOG_H
#define FAIRYTALESDIALOG_H

#include <QDialog>

#include "fairytale.h"
#include "ui_fairytalesdialog.h"

class FairytalesDialog : public QDialog, protected Ui::FairytalesDialog
{
	Q_OBJECT

	public:
		explicit FairytalesDialog(fairytale *app, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
		virtual ~FairytalesDialog();

	private slots:
		void playFairytale();
		void deleteFairytale();

	protected:
		virtual void showEvent(QShowEvent *event);

	private:
		class Fairytale : public QObject
		{
			public:
				Fairytale(const QString &key, QWidget *widget, QObject *parent);
				~Fairytale();

				QString key() const;

			private:
				QString m_key;
				QWidget *m_widget;
		};

		void clearFairytales();
		void update();

		fairytale *m_app;
		typedef QMap<QWidget*, Fairytale*> Fairytales;
		Fairytales m_fairytales;
};

#endif // FAIRYTALESDIALOG_H
