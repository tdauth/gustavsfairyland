#ifndef BONUSCLIPSDIALOG_H
#define BONUSCLIPSDIALOG_H

#include <QDialog>

#include "fairytale.h"
#include "ui_bonusclipsdialog.h"

class BonusClipsDialog : public QDialog, protected Ui::BonusClipsDialog
{
	Q_OBJECT

	public:
		explicit BonusClipsDialog(fairytale *app, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
		virtual ~BonusClipsDialog();


	private slots:
		void playBonusClip();

	protected:
		virtual void showEvent(QShowEvent *event) override;
		virtual void changeEvent(QEvent *event) override;

	private:
		class Button : public QObject
		{
			public:
				Button(const fairytale::ClipKey &clipKey, QPushButton *button, QObject *parent);
				~Button();

				fairytale::ClipKey clipKey() const;

			private:
				fairytale::ClipKey m_clipKey;
				QPushButton *m_button;
		};

		void clearButtons();
		void update();

		fairytale *m_app;
		typedef QMap<QPushButton*, Button*> Buttons;
		Buttons m_buttons;
};

#endif // BONUSCLIPSDIALOG_H
