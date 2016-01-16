#include "fairytale.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtMultimedia/QMultimedia>

#include <QtCore/QXmlStreamReader>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include <iostream>

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

	QList<Clip*> clips;

	if (loadClipsFromFile(file, clips))
	{
		this->m_clips = clips;
		qDebug() << "start";
		nextTurn();
	}
	else
	{
		foreach (Clip *clip, clips)
		{
			delete clip;
		}

		qDebug() << this->m_clips.size() << " clips";
	}
}

void fairytale::pauseGame()
{
	if (this->m_paused)
	{
		this->actionPauseGame->setText(tr("Pause Game"));
		this->m_player->pauseButton()->setText(tr("Pause Game"));
		this->m_paused = false;

		if (this->m_remainingTime > 0)
		{
			this->m_timer.start();

			foreach (QPushButton *button, this->m_buttons)
			{
				button->setEnabled(true);
			}
		}
		else
		{
			this->m_player->mediaPlayer()->play();
		}
	}
	else
	{
		this->actionPauseGame->setText(tr("Continue Game"));
		this->m_player->pauseButton()->setText(tr("Continue Game"));
		this->m_paused = true;

		if (this->m_remainingTime > 0)
		{
			this->m_timer.stop();

			foreach (QPushButton *button, this->m_buttons)
			{
				button->setEnabled(false);
			}
		}
		else
		{
			this->m_player->mediaPlayer()->pause();
		}
	}
}

fairytale::fairytale() : m_remainingTime(0), m_currentSolution(0), m_playCompleteSolution(false), m_completeSolutionIndex(0), m_player(new Player(this, this)), m_requiresPerson(true), m_paused(false)
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
	connect(actionPauseGame, SIGNAL(triggered()), this, SLOT(pauseGame()));
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

	this->m_player->playVideo(this, this->m_completeSolution[index]->narratorVideoUrl());
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

		if (!this->m_currentClips.empty())
		{
			this->selectRandomSolution();

			qDebug() << "Complete size " << m_clips.size();

			/*
			* Play the narrator clip for the current solution as hint.
			*/
			this->m_player->playVideo(this, this->m_currentSolution->narratorVideoUrl());
		}
		else
		{
			QMessageBox::information(this, tr("WIN!"), tr("You won the game!!!!"), QMessageBox::Ok);
			this->playFinalVideoPushButton->setEnabled(true);
		}
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
	this->m_remainingTime = 0; // set time to 0 that the method pauseGame() works
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

	/*
	 * Allow only persons or acts.
	 */
	for (int i = 0; i < copy.size(); )
	{
		if ((m_requiresPerson && !copy[i]->isPerson()) || (!m_requiresPerson && copy[i]->isPerson()))
		{
			copy.removeAt(i);
		}
		else
		{
			++i;
		}
	}

	if (copy.isEmpty())
	{
		return;
	}

	m_requiresPerson = !m_requiresPerson;

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

bool fairytale::loadClipsFromFile(const QString &file, QList<Clip*> &clips)
{
	QFile f(file);

	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Unable to open file" << file;

		return false;
	}

	QDomDocument document;

	if (!document.setContent(&f))
	{
		std::cerr << "Error on reading DOM tree" << std::endl;

		return false;
	}

	std::cerr << document.nodeName().toUtf8().constData() << std::endl;

	const QDomElement root = document.firstChildElement();

	if (root.nodeName() != "clips")
	{
		std::cerr << "Missing <clips>" << std::endl;

		return false;
	}

	QDomNodeList nodes = root.elementsByTagName("clip");

	if (nodes.isEmpty())
	{
		std::cerr << "Missing clips" << std::endl;

		return false;
	}

	for (int i = 0; i < nodes.size(); ++i)
	{
		QDomNode node = nodes.at(i);

		if (node.nodeName() != "clip")
		{
			std::cerr << "Missing clip" << std::endl;

			return false;
		}

		const QUrl image = QUrl(node.firstChildElement("image").text());
		const QUrl video = QUrl(node.firstChildElement("video").text());
		const QUrl narrator = QUrl(node.firstChildElement("narrator").text());

		const QString isPerson = node.hasAttributes() && node.attributes().contains("isPerson") ?  node.attributes().namedItem("isPerson").nodeValue() : "";

		clips.push_back(new Clip(image, video, narrator, isPerson == "true", this));
	}

	std::cerr << "Clips " << clips.size() << std::endl;

	return true;
}

QUrl fairytale::resolveClipUrl(const QUrl& url) const
{
	if (!url.isRelative())
	{
		return url;
	}

	QUrl result = this->m_clipsDir;
	result.setUrl(this->m_clipsDir.url() + "/" + url.url());

	return result;
}



#include "fairytale.moc"
