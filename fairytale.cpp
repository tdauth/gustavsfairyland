#include "fairytale.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtMultimedia/QMultimedia>

#include <QtCore/QXmlStreamReader>

#include "clip.h"
#include "player.h"

void fairytale::newGame()
{
	this->m_timer.stop();

	clearSolution();
	clearClips();

	const QString file = QFileDialog::getOpenFileName(this, tr("Clips"), QString(), "XML files (*.xml);;All files (*)");

	if (file.isEmpty())
	{
		qDebug() << "no file selected";

		return;
	}

	qDebug() << "After selecting file";

	if (loadClipsFromFile(file))
	{
		qDebug() << "start";
		nextTurn();
	}
	else
	{
		qDebug() << this->m_clips.size() << " clips";
	}
}

fairytale::fairytale() : m_remainingTime(0), m_currentSolution(0), m_playCompleteSolution(false), m_completeSolutionIndex(0), m_player(new Player(this))
{
	this->m_player->hide();

	setupUi(this);


	connect(&this->m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));

	this->m_buttons.push_back(this->card0PushButton);
	this->m_buttons.push_back(this->card1PushButton);
	this->m_buttons.push_back(this->card2PushButton);
	this->m_buttons.push_back(this->card3PushButton);

	for (int i = 0; i < this->m_buttons.size(); ++i)
	{
		connect(this->m_buttons[i], SIGNAL(clicked()), this, SLOT(clickCard()));
	}

	connect(actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(this->m_player->mediaPlayer(), SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(finishNarrator(QMediaPlayer::State)));
	connect(this->playFinalVideoPushButton, SIGNAL(clicked()), this, SLOT(playFinalVideo()));
}

fairytale::~fairytale()
{
}

void fairytale::playFinalClip(int index)
{
	this->m_completeSolutionIndex = index;

	this->m_player->playVideo(this->m_completeSolution[index]->narratorVideoUrl());
}

void fairytale::playFinalVideo()
{
	if (m_playCompleteSolution)
	{
		return;
	}

	this->m_playCompleteSolution = true;
	this->playFinalClip(0);
}

void fairytale::about()
{
	QMessageBox::information(this, tr("About"), tr("This game has been created by Tamino Dauth and Carsten Thomas. It is the best game you will ever play!"));
}

void fairytale::gameOver()
{
	this->clear();
	QMessageBox::information(this, tr("Game over!"), tr("GAME OVER!"));
}

void fairytale::nextTurn()
{
	this->timeLabel->setText(tr("Time"));

	this->clear();

	if (!this->m_clips.empty())
	{
		this->fillCurrentClips();


		this->selectRandomSolution();

		qDebug() << "Complete size " << m_clips.size();

		/*
		 * Play the narrator clip for the current solution as hint.
		 */
		this->m_player->playVideo(this->m_currentSolution->narratorVideoUrl());
	}
	else
	{
		QMessageBox::information(this, tr("WIN!"), tr("You won the game!!!!"), QMessageBox::Ok);
		this->playFinalVideoPushButton->setEnabled(true);
	}
}

void fairytale::clear()
{
	for (int i = 0; i < this->m_currentClips.size(); ++i)
	{
		this->m_buttons[i]->setIcon(QIcon());
		this->m_buttons[i]->setEnabled(false);
	}

	this->m_currentClips.clear();
	this->m_currentSolution = 0;
}

void fairytale::clearSolution()
{
	this->playFinalVideoPushButton->setEnabled(false);
	this->m_playCompleteSolution = false;
	this->m_completeSolutionIndex = 0;
	this->m_completeSolution.clear();

	foreach (QPushButton *button, this->m_completeSolutionButtons)
	{
		delete button;
	}

	this->m_completeSolutionButtons.clear();
}

void fairytale::clearClips()
{
	foreach (Clip *clip, this->m_clips)
	{
		delete clip;
	}

	this->m_clips.clear();
}

void fairytale::finishNarrator(QMediaPlayer::State state)
{
	if (state == QMediaPlayer::StoppedState)
	{
		if (!this->m_playCompleteSolution)
		{
			this->m_player->mediaPlayer()->stop();
			this->m_remainingTime = 2000 * (this->m_clips.size() + 1);
			this->updateTimeLabel();

			for (int i = 0; i < this->m_currentClips.size(); ++i)
			{
				this->m_buttons[i]->setIcon(QIcon(this->m_currentClips[i]->imageUrl().toLocalFile()));
				this->m_buttons[i]->setEnabled(true);
			}

			// run every second
			this->m_timer.start(1000);
		}
		else if (!this->m_player->skipped() && this->m_completeSolutionIndex + 1 < this->m_completeSolution.size())
		{
			this->playFinalClip(this->m_completeSolutionIndex + 1);
		}
		/*
		 * Stop playing the complete solution.
		 */
		else
		{
			this->m_playCompleteSolution = false;
			this->m_completeSolutionIndex = 0;
			QMessageBox::information(this, tr("Finish"), tr("Finish"));
		}
	}
}

void fairytale::timerTick()
{
	qDebug() << "Tick";
	this->m_remainingTime -= 1000;
	this->updateTimeLabel();

	if (this->m_remainingTime <= 0)
	{
		this->m_timer.stop();

		this->gameOver();
	}
}

void fairytale::clickCard()
{
	this->m_timer.stop();
	bool success = false;

	for (int i = 0; i < this->m_currentClips.size() && !success; ++i)
	{
		//if (QObject::sender() == this->m_buttons[i] && this->m_currentSolution == this->m_currentClips[i]) {
			addCurrentSolution();
			nextTurn();

			success = true;
		// }
	}

	if (!success)
	{
		this->gameOver();
	}
}

void fairytale::updateTimeLabel()
{
	this->timeLabel->setText(tr("%1 Seconds").arg(QString::number(this->m_remainingTime / 1000)));
}

void fairytale::addCurrentSolution()
{
	QPushButton *button = new QPushButton(this);
	button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	button->setIcon(QIcon(this->m_currentSolution->imageUrl().toLocalFile()));
	button->setEnabled(false);
	this->scrollAreaWidgetContents->layout()->addWidget(button);
	this->m_completeSolutionButtons.push_back(button);
	this->m_completeSolution.push_back(this->m_currentSolution);
}

void fairytale::fillCurrentClips()
{
	QList<Clip*> copy = this->m_clips;

	qDebug() << "Size before" << copy.size();

	for (int i = 0; i < this->m_buttons.size() && !copy.empty(); ++i)
	{
		const int index = qrand() % copy.size();
		this->m_currentClips.push_back(copy[index]);
		copy.removeAt(index);
		//qDebug() << "Index: " << index << " and size " << copy.size();
	}

	qDebug() << "Current size of buttons " << this->m_currentClips.size();
}

void fairytale::selectRandomSolution()
{
	const int index = qrand() % this->m_currentClips.size();
	Clip *solution = this->m_currentClips[index];
	this->m_currentSolution = solution;
	this->m_clips.removeAll(solution); // solution is done forever
}

bool fairytale::loadClipsFromFile(const QString &file)
{
	this->m_clips.clear();
	QFile f(file);

	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Unable to open file" << file;

		return false;
	}

	QXmlStreamReader xmlReader(&f);

	if (!xmlReader.readNextStartElement())
	{
		qDebug() << "Missing start element";

		return false;
	}

	if (xmlReader.name() != "clips")
	{
		qDebug() << "Missing <clips> element";

		return false;
	}

	while (xmlReader.readNext())
	{
		QUrl image, video, narrator;

		QMessageBox::information(this, xmlReader.name().toString(), xmlReader.readElementText());

		if (xmlReader.name() != "clip")
		{
			qDebug() << "Missing <clip> element";

			return false;
		}

		if (!xmlReader.readNext())
		{
			qDebug() << "Missing <image> element";

			return false;
		}

		if (xmlReader.name() != "image") {
			qDebug() << "Missing <image> element";

			return false;
		}

		image = QUrl(xmlReader.text().toString());

		if (!xmlReader.readNext()) {
			qDebug() << "Missing <video> element";

			return false;
		}

		if (xmlReader.name() != "video") {
			qDebug() << "Missing <video> element";

			return false;
		}

		video = QUrl(xmlReader.text().toString());

		if (!xmlReader.readNext()) {
			qDebug() << "Missing <narrator> element";

			return false;
		}

		if (xmlReader.name() != "narrator") {
			qDebug() << "Missing <narrator> element";

			return false;
		}

		narrator = QUrl(xmlReader.text().toString());

		this->m_clips.push_back(new Clip(image, video, narrator, this));
	}

	qDebug() << this->m_clips.size() << " clips";

	return true;
}


#include "fairytale.moc"
