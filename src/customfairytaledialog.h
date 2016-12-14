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
		void retryWithDifferentGameMode();
		void retryEasier();
		void retryHarder();

	public:
		CustomFairytaleDialog(fairytale *app, QWidget *parent);

		void clear();
		/**
		 * \return If this returns try it indicates that the user wants to retry the game with the same parameters as before.
		 */
		bool clickedRetry() const;
		GameMode* retryGameMode() const;
		fairytale::Difficulty retryDifficulty() const;

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
		GameMode *m_retryGameMode;
		fairytale::Difficulty m_retryDifficulty;
};

inline bool CustomFairytaleDialog::clickedRetry() const
{
	return this->m_retry;
}

inline GameMode* CustomFairytaleDialog::retryGameMode() const
{
	return this->m_retryGameMode;
}

inline fairytale::Difficulty CustomFairytaleDialog::retryDifficulty() const
{
	return this->m_retryDifficulty;
}

#endif // CUSTOMFAIRYTALEDIALOG_H
