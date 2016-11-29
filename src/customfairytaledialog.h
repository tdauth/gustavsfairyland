#ifndef CUSTOMFAIRYTALEDIALOG_H
#define CUSTOMFAIRYTALEDIALOG_H

#include <QDialog>

#include "ui_customfairytale.h"
#include "fairytale.h"
#include "customfairytale.h"

class Clip;
class IconLabel;

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
		void addClip(const fairytale::ClipKey &clipKey);
		void save();
		void retry();

	public:
		CustomFairytaleDialog(fairytale *app, QWidget *parent);

		void clear();
		bool clickedRetry() const;

	protected:
		virtual void showEvent(QShowEvent *event) override;
		virtual void changeEvent(QEvent *event) override;

	private:
		typedef QList<fairytale::ClipKey> Clips;
		typedef QList<IconLabel*> ClipLabels;

		fairytale *m_app;
		Clips m_clips;
		ClipLabels m_clipLabels;
		bool m_retry;
};

inline bool CustomFairytaleDialog::clickedRetry() const
{
	return this->m_retry;
}

#endif // CUSTOMFAIRYTALEDIALOG_H
