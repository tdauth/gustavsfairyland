#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <QDialog>
#include <QLinkedList>
#include <QHash>

#include "ui_highscores.h"
#include "fairytale.h"

/**
 * \brief A single highscore entry is defined by the gamemode, package, the number of rounds, the taken time and the player's name.
 */
class HighScore
{
	public:
		HighScore();
		HighScore(const QString &name, const QString &package, const QString &gameMode, fairytale::Difficulty difficulty, int rounds, int time);

		const QString& name() const;
		const QString& package() const;
		const QString& gameMode() const;
		fairytale::Difficulty difficulty() const;
		int rounds() const;
		int time() const;

	private:
		QString m_name;
		QString m_package;
		QString m_gameMode;
		fairytale::Difficulty m_difficulty;
		int m_rounds;
		int m_time;
};

inline const QString& HighScore::name() const
{
	return this->m_name;
}

inline const QString& HighScore::package() const
{
	return this->m_package;
}

inline const QString& HighScore::gameMode() const
{
	return this->m_gameMode;
}

inline fairytale::Difficulty HighScore::difficulty() const
{
	return this->m_difficulty;
}

inline int HighScore::rounds() const
{
	return this->m_rounds;
}

inline int HighScore::time() const
{
	return this->m_time;
}

class fairytale;


/**
 * \brief Modal dialog to list all highscores. The highscores for each package with a game mode are limited to \ref maxHighScores.
 */
class HighScores : public QDialog, protected Ui::HighScores
{
	Q_OBJECT

	public:
		/// Store high scores with the samge package and game mode, comparing highscores from different packages or game modes does not make any sense.
		typedef QPair<QString, QString> Key;
		typedef QLinkedList<HighScore> HighScoreList;
		typedef QHash<Key, HighScoreList> HighScoreMap;

		static const int maxHighScores = 10;

		HighScores(fairytale *app, QWidget *parent);

		fairytale* app() const;

		bool addHighScore(const HighScore &highScore);
		const HighScoreMap& highScores() const;

		static QString difficultyToString(fairytale::Difficulty difficulty);

	protected:
		virtual void changeEvent(QEvent *event) override;
		virtual void showEvent(QShowEvent *event) override;

	private:
		fairytale *m_app;
		HighScoreMap m_highScores;

};

inline fairytale* HighScores::app() const
{
	return this->m_app;
}

inline const HighScores::HighScoreMap& HighScores::highScores() const
{
	return this->m_highScores;
}

#endif // HIGHSCORES_H
