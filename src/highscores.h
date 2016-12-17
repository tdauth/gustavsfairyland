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
		HighScore(const QString &name, const QStringList &packages, const QString &gameMode, fairytale::Difficulty difficulty, int rounds, int time);

		const QString& name() const;
		/**
		 * All packages which have been used.
		 */
		const QStringList& packages() const;
		const QString& gameMode() const;
		fairytale::Difficulty difficulty() const;
		int rounds() const;
		int time() const;

	private:
		QString m_name;
		QStringList m_packages;
		QString m_gameMode;
		fairytale::Difficulty m_difficulty;
		int m_rounds;
		int m_time;
};

inline const QString& HighScore::name() const
{
	return this->m_name;
}

inline const QStringList& HighScore::packages() const
{
	return this->m_packages;
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

	public slots:
		void clearAll();

	public:
		/// Store high scores with the same game mode, comparing highscores from different game modes does not make any sense. The packages do not matter since they just add clips.
		typedef QString Key;
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
