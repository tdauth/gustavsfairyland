#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <QDialog>
#include <QHash>

#include "ui_highscores.h"

class HighScore
{
	public:
		HighScore();
		HighScore(const QString &name, const QString &package, const QString &gameMode, int rounds, int time);

		const QString& name() const;
		const QString& package() const;
		const QString& gameMode() const;
		int rounds() const;
		int time() const;

	private:
		QString m_name;
		QString m_package;
		QString m_gameMode;
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

inline int HighScore::rounds() const
{
	return this->m_rounds;
}

inline int HighScore::time() const
{
	return this->m_time;
}

class HighScores : public QDialog, protected Ui::HighScores
{
	public:
		/// Store high scores with the samge package and game mode, comparing highscores from different packages or game modes does not make any sense.
		typedef QPair<QString, QString> Key;
		typedef QVector<HighScore> HighScoreVector;
		typedef QHash<Key, HighScoreVector> HighScoreMap;

		static const int maxHighScores = 10;;

		HighScores(QWidget *parent);

		bool addHighScore(const HighScore &highScore);
		const HighScoreMap& highScores() const;

	protected:
		virtual void changeEvent(QEvent *event) override;
		virtual void showEvent(QShowEvent *event) override;

	private:
		HighScoreMap m_highScores;

};

inline const HighScores::HighScoreMap& HighScores::highScores() const
{
	return this->m_highScores;
}

#endif // HIGHSCORES_H
