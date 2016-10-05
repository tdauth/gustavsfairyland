#ifndef GAMEMODECREATIVE_H
#define GAMEMODECREATIVE_H

#include <QVector>
#include <QPushButton>
#include <QLabel>

#include "gamemode.h"

class IconButton;

class GameModeCreative : public GameMode
{
	Q_OBJECT

	public slots:
		void clickCard();
		void finish();

	public:
		GameModeCreative(fairytale *app);

		virtual GameMode::State state() override;
		virtual Clip* solution() override;
		virtual long int time() override;
		virtual void afterNarrator() override;
		virtual void nextTurn() override;
		virtual void resume() override;
		virtual void pause() override;
		virtual void end() override;
		virtual void start() override;
		virtual QString name() const override;
		virtual QString id() const override;

		virtual bool hasToChooseTheSolution() override;
		virtual bool hasLimitedTime() override;
		virtual bool playIntro() override;
		virtual bool playOutro() override;
		virtual bool showWinDialog() override;
		virtual bool unlockBonusClip() override;
		virtual bool addToHighScores() override;

	private:
		class ClipButton : public QWidget
		{
			public:
				ClipButton(QWidget *parent, Clip *clip);

				Clip* clip() const;
				IconButton* iconButton() const;
				QLabel* label() const;

			private:
				Clip *m_clip;
				IconButton *m_iconButton;
				QLabel *m_label;
		};

		void setState(State state);

		State m_state;
		Clip *m_currentSolution;
		/**
		 * All clip buttons from which the user has to choose one solution.
		 */
		QVector<ClipButton*> m_buttons;
		QPushButton *m_finishButton;
};

inline Clip* GameModeCreative::ClipButton::clip() const
{
	return this->m_clip;
}

inline IconButton* GameModeCreative::ClipButton::iconButton() const
{
	return this->m_iconButton;
}

inline QLabel* GameModeCreative::ClipButton::label() const
{
	return this->m_label;
}

#endif // GAMEMODECREATIVE_H
