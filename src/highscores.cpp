#include <QtGui>
#include <QtWidgets>

#include "highscores.h"
#include "fairytale.h"
#include "gamemode.h"
#include "clippackage.h"

HighScore::HighScore() : m_rounds(0), m_time(0)
{
}

HighScore::HighScore(const QString &name, const QStringList &packages, const QString &gameMode, fairytale::Difficulty difficulty, int rounds, int time) : m_name(name), m_packages(packages), m_gameMode(gameMode), m_difficulty(difficulty), m_rounds(rounds), m_time(time)
{
}

void HighScores::clearAll()
{
	if (QMessageBox::question(this, tr("Clear all High Scores?"), tr("Do you really want to clear all High Scores?")) == QMessageBox::Yes)
	{
		this->tableWidget->clearContents();
		this->tableWidget->setRowCount(0);
		this->m_highScores.clear();
	}
}

HighScores::HighScores(fairytale *app, QWidget *parent) : TranslatedWidget(app, parent)
{
	setupUi(this);

	this->setModal(true);

	connect(this->okPushButton, &QPushButton::clicked, this, &QDialog::accept);
	connect(this->clearPushButton, &QPushButton::clicked, this, &HighScores::clearAll);
}

bool HighScores::addHighScore(const HighScore &highScore)
{
	const Key key(highScore.gameMode());

	if (!this->m_highScores.contains(key))
	{
		this->m_highScores[key] = HighScoreList();
	}

	HighScoreList &existing = this->m_highScores[key];
	HighScoreList::iterator pos = existing.begin();

	// find a highscore which is worse an insert it before, otherwise if none is found it will be inserted at the front (position 0)
	for ( ; pos != existing.end(); ++pos)
	{
		const HighScore &existingHighScore = *pos;

		// existingHighScore is worse when it took longer but with the same number of rounds and difficulty. The game mode is already the same. It is also worse if it has lesse rounds with the same difficulty or if simply the difficulty is lower.
		if ((existingHighScore.time() > highScore.time() && existingHighScore.rounds() == highScore.rounds() && existingHighScore.difficulty() == highScore.difficulty()) || (existingHighScore.rounds() < highScore.rounds() && existingHighScore.difficulty() == highScore.difficulty()) || ((int)existingHighScore.difficulty() < (int)highScore.difficulty()))
		{
			qDebug() << "Found smaller highscore with time: " << existingHighScore.time() << " and rounds " << existingHighScore.rounds() << " and difficulty " << (int)existingHighScore.difficulty();
			qDebug() << "This highscore with time: " << highScore.time() << " and rounds " << highScore.rounds() << " and difficulty " << (int)highScore.difficulty();

			break;
		}
	}

	// Insert it before the pos. If no highscore was worse, it is existing.end() and will be appended to the end.
	existing.insert(pos, highScore);

	// Drop all highscores at the end. There is a limit per key (Package and Game Mode).
	if (existing.size() > maxHighScores)
	{
		qDebug() << "Resize it is too big";

		for (int i = existing.size(); i > maxHighScores; --i)
		{
			existing.pop_back();
		}
	}

	return true;
}

void HighScores::showEvent(QShowEvent *event)
{
	TranslatedWidget::showEvent(event);

	this->tableWidget->setRowCount(0);
	int row = 0;

	qDebug() << m_highScores.values().size() << " is the size of vectors";

	foreach (const HighScoreList &highScoreList, m_highScores.values())
	{
		qDebug() << highScoreList.size() << "Is the size of the vector";

		foreach (const HighScore &highScore, highScoreList)
		{
			this->tableWidget->setRowCount(row + 1);
			QTableWidgetItem *newItem = nullptr;

			const fairytale::GameModes::const_iterator gameModeIterator = app()->gameModes().find(highScore.gameMode());
			const QString gameModeName = gameModeIterator != app()->gameModes().end() ? gameModeIterator.value()->name() : highScore.gameMode();

			const QStringList packages = highScore.packages();
			QString packagesText;
			int i = 0;

			foreach (const QString &packageId, packages)
			{
				const fairytale::ClipPackages::const_iterator packageIterator = app()->clipPackages().find(packageId);

				const QString packageName = packageIterator != app()->clipPackages().end() ? packageIterator.value()->name() : packageId;

				if (i == 0)
				{
					packagesText += tr("%1").arg(packageName);
				}
				else
				{
					packagesText += tr(", %1").arg(packageName);
				}

				++i;
			}

			newItem = new QTableWidgetItem(tr("%1.").arg(row + 1));
			tableWidget->setItem(row, 0, newItem);
			newItem = new QTableWidgetItem(highScore.name());
			tableWidget->setItem(row, 1, newItem);
			newItem = new QTableWidgetItem(difficultyToString(highScore.difficulty()));
			tableWidget->setItem(row, 2, newItem);
			newItem = new QTableWidgetItem(QString::number(highScore.rounds()));
			tableWidget->setItem(row, 3, newItem);
			newItem = new QTableWidgetItem(QString::number(highScore.time() / 1000)); // show time in seconds
			tableWidget->setItem(row, 4, newItem);
			newItem = new QTableWidgetItem(gameModeName);
			tableWidget->setItem(row, 5, newItem);
			newItem = new QTableWidgetItem(packagesText);
			tableWidget->setItem(row, 6, newItem);

			++row;
		}
	}

	// Stretch the last column.
	for (int i = 0; i < 6; ++i)
	{
		tableWidget->resizeColumnToContents(i);
	}

	tableWidget->resizeRowsToContents();
}

QString HighScores::difficultyToString(fairytale::Difficulty difficulty)
{
	switch (difficulty)
	{
		case fairytale::Difficulty::Easy:
			return tr("Easy");

		case fairytale::Difficulty::Normal:
			return tr("Normal");

		case fairytale::Difficulty::Hard:
			return tr("Hard");

		case fairytale::Difficulty::Mahlerisch:
			return tr("Mahlerisch");
	}

	return QString();
}
