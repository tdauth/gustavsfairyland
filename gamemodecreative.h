#ifndef GAMEMODECREATIVE_H
#define GAMEMODECREATIVE_H

#include <QVector>
#include <QPushButton>

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
		virtual bool hasLimitedTime() override;
		virtual long int time() override;
		virtual void afterNarrator() override;
		virtual bool hasToChooseTheSolution() override;
		virtual void nextTurn() override;
		virtual void resume() override;
		virtual void pause() override;
		virtual void end() override;
		virtual void start() override;
		virtual QString name() const override;
		virtual QString id() const override;

	private:
		void setState(State state);

		State m_state;
		Clip *m_currentSolution;
		/**
		 * All clip buttons from which the user has to choose one solution.
		 */
		QVector<IconButton*> m_buttons;
		QVector<Clip*> m_clips;
		QPushButton *m_finishButton;
};

#endif // GAMEMODECREATIVE_H
