#ifndef LOCALEDIALOG_H
#define LOCALEDIALOG_H

#include <QDialog>
#include <QRadioButton>

#include "fairytale.h"
#include "ui_localedialog.h"

class LocaleDialog : public QDialog, protected Ui::LocaleDialog
{
	Q_OBJECT

	public:
		explicit LocaleDialog(fairytale *app, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	private slots:
		void changeLocale();

	protected:
		virtual void showEvent(QShowEvent *event);
		virtual void changeEvent(QEvent *event) override;

	private:
		class Button : public QObject
		{
			public:
				Button(const QString &locale, QRadioButton *button, QObject *parent);
				~Button();

				QString locale() const;

			private:
				QString m_locale;
				QRadioButton *m_button;
		};

		void update();

		fairytale *m_app;
		typedef QMap<QRadioButton*, Button*> Buttons;
		Buttons m_buttons;
};

#endif // LOCALEDIALOG_H
