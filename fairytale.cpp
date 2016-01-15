#include "fairytale.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>

#include <Phonon/MediaObject>
#include <Phonon/MediaSource>
#include <Phonon/VideoPlayer>
#include <Phonon/VideoWidget>
#include <Phonon/SeekSlider>
#include <Phonon/AudioOutput>

#include "clip.h"

void fairytale::newGame()
{
	this->m_timer.stop();

	clearSolution();
	clearClips();

	for (int i = 0; i < 6; ++i)
	{
		this->m_clips.push_back(new Clip(QUrl("file:///home/tamino/Desktop/tumblr_mzhy6wtilI1s73qs0o1_500.gif"), QUrl("file:///home/tamino/Dokumente/Projekte/Film (TaCaMo)/output.mp4"), QUrl("file:///home/tamino/Dokumente/Projekte/Film (TaCaMo)/output.mp4"), this));
	}

	nextTurn();
}

void fairytale::skipNarrator()
{
	//this->narratorVideoPlayer->stop();
	this->narratorVideoPlayer->seek(this->narratorVideoPlayer->totalTime());
}

fairytale::fairytale() : m_remainingTime(0), m_currentSolution(0), m_playCompleteSolution(false), m_completeSolutionIndex(0)
{
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

	connect(this->skipNarratorPushButton, SIGNAL(clicked()), this, SLOT(skipNarrator()));
	connect(this->narratorVideoPlayer, SIGNAL(finished()), this, SLOT(finishNarrator()));

	connect(actionNewGame, SIGNAL(triggered()), this, SLOT(newGame()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
}

fairytale::~fairytale()
{
}

void fairytale::playFinalClip(int index)
{
	this->m_completeSolutionIndex = index;

	this->playVideo(this->m_completeSolution[index]->narratorVideoUrl());
}

void fairytale::playFinalVideo()
{
	this->m_playCompleteSolution = true;
	this->playFinalClip(0);
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
		this->playVideo(this->m_currentSolution->narratorVideoUrl());
	}
	else
	{
		if (QMessageBox::question(this, tr("WIN!"), tr("Play the final video?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			this->playFinalVideo();
		}
		else
		{
			clearSolution();
		}
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

void fairytale::playVideo(const QUrl& url)
{
	this->skipNarratorPushButton->setEnabled(true);
	/*
	 * Play the narrator clip for the current solution as hint.
	 */
	Phonon::MediaSource source(url);
	this->narratorVideoPlayer->load(source);
	this->seekSlider->setMediaObject(this->narratorVideoPlayer->mediaObject());
	this->volumeSlider->setAudioOutput(this->narratorVideoPlayer->audioOutput());
	this->narratorVideoPlayer->play();
}

void fairytale::finishNarrator()
{
	this->skipNarratorPushButton->setEnabled(false);

	if (!this->m_playCompleteSolution)
	{
		this->narratorVideoPlayer->stop();
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
	else if (this->m_completeSolutionIndex + 1 < this->m_completeSolution.size())
	{
		this->playFinalClip(this->m_completeSolutionIndex + 1);
	}
	else
	{
		this->m_playCompleteSolution = false;
		this->m_completeSolutionIndex = 0;
		QMessageBox::information(this, tr("Finish"), tr("Finish"));
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
	button->setIconSize(QSize(64, 64));
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

#include "fairytale.moc"
