#include <QtGui>
#include <QPushButton>

#include "highscores.h"

HighScore::HighScore() : m_rounds(0), m_time(0)
{
}

HighScore::HighScore(const QString &name, const QString &package, const QString &gameMode, int rounds, int time) : m_name(name), m_package(package), m_gameMode(gameMode), m_rounds(rounds), m_time(time)
{
}

HighScores::HighScores(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	connect(this->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
}

bool HighScores::addHighScore(const HighScore &highScore)
{
	Key key(highScore.package(), highScore.gameMode());
	HighScoreVector existing = this->m_highScores[key];
	int pos = 0;

	for (int i = 0; i < existing.size(); ++i)
	{
		if (existing[i].time() > highScore.time() && existing[i].rounds() <= highScore.rounds())
		{
			pos = i;

			break;
		}
	}

	if (pos >= maxHighScores)
	{
		qDebug() << pos << "is too big!!";

		return false;
	}

	existing.insert(pos, highScore);

	if (existing.size() > maxHighScores)
	{
		qDebug() << "Resize it is too big";

		existing.resize(maxHighScores);
	}

	this->m_highScores[key] = existing;

	return true;
}

void HighScores::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	this->tableWidget->setRowCount(0);
	int row = 0;

	qDebug() <<  m_highScores.values().size() << " is the size of vectors";

	foreach (const HighScoreVector &highScoreVector, m_highScores.values())
	{
		qDebug() << highScoreVector.size() << "Is the size of the vector";

		foreach (const HighScore &highScore, highScoreVector)
		{
			this->tableWidget->setRowCount(row + 1);
			QTableWidgetItem *newItem = new QTableWidgetItem(highScore.name());
			tableWidget->setItem(row, 0, newItem);
			newItem = new QTableWidgetItem(highScore.package());
			tableWidget->setItem(row, 1, newItem);
			newItem = new QTableWidgetItem(highScore.gameMode());
			tableWidget->setItem(row, 2, newItem);
			newItem = new QTableWidgetItem(QString::number(highScore.rounds()));
			tableWidget->setItem(row, 3, newItem);
			newItem = new QTableWidgetItem(QString::number(highScore.time()));
			tableWidget->setItem(row, 4, newItem);
			++row;
		}
	}
}
