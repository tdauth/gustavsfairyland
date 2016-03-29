#include "fairytale.h"

#include <QtCore/QSettings>

#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtMultimedia/QMultimedia>

#include "clip.h"
#include "player.h"
#include "iconbutton.h"
#include "clipsdialog.h"
#include "clippackagedialog.h"
#include "clippackage.h"
#include "clippackageeditor.h"
#include "customfairytaledialog.h"

void fairytale::newGame()
{
	this->m_timer.stop();

	clearSolution();
	clearClips();

	this->m_paused = false;
	this->actionPauseGame->setText(tr("Pause Game"));
	this->m_player->pauseButton()->setText(tr("Pause Game"));

	// requires person in the first step always
	this->m_requiresPerson = true;
	this->m_turns = 0;

	ClipPackage *clipPackage = this->selectClipPackage();

	if (clipPackage != nullptr)
	{
		this->m_clipPackage = clipPackage;
		this->m_clips = clipPackage->clips();

		qDebug() << "start";

		nextTurn();

		setGameButtonsEnabled(true);
	}

	this->m_isRunning = true;
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

void fairytale::showCustomFairytale()
{
	if (!this->m_paused)
	{
		pauseGame();
	}

	this->customFairytaleDialog()->show();
}

void fairytale::openClipsDialog()
{
	if (this->m_clipsDialog == nullptr)
	{
		this->m_clipsDialog = new ClipsDialog(this, this);
	}

	this->m_clipsDialog->fill(this->clipPackages());
	this->m_clipsDialog->show();
}

void fairytale::openEditor()
{
	if (this->m_editor == nullptr)
	{
		this->m_editor = new ClipPackageEditor(this, this);
	}

	this->m_editor->show();
}

ClipPackage* fairytale::selectClipPackage()
{
	if (this->m_clipPackageDialog == nullptr)
	{
		this->m_clipPackageDialog = new ClipPackageDialog(this);
	}

	this->m_clipPackageDialog->fill(this->clipPackages());

	if (this->m_clipPackageDialog->exec() == QDialog::Accepted)
	{
		return this->m_clipPackageDialog->clipPackage();
	}

	return nullptr;
}

fairytale::fairytale()
: m_turns(0)
, m_startPerson(nullptr)
, m_player(new Player(this, this))
, m_clipsDialog(nullptr)
, m_clipPackageDialog(nullptr)
, m_editor(nullptr)
, m_customFairytaleDialog(nullptr)
, m_remainingTime(0)
, m_requiresPerson(true)
, m_clipPackage(nullptr)
, m_currentSolution(nullptr)
, m_completeSolutionIndex(0)
, m_playCompleteSolution(false)
, m_paused(false)
, m_isRunning(false)
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
	connect(actionShowCustomFairytale, SIGNAL(triggered()), SLOT(showCustomFairytale()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionClips, SIGNAL(triggered()), this, SLOT(openClipsDialog()));
	connect(actionEditor, SIGNAL(triggered()), this, SLOT(openEditor()));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(this->m_player->mediaPlayer(), SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(finishNarrator(QMediaPlayer::State)));

	QSettings settings("fairytale");
	const int size = settings.beginReadArray("clipPackages");

	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		const QString filePath = settings.value("filePath").toString();

		ClipPackage *package = new ClipPackage(this);

		if (package->loadClipsFromFile(filePath))
		{
			this->m_clipPackages.push_back(package);
		}
		else
		{
			delete package;
		}
	}

	settings.endArray();
}

fairytale::~fairytale()
{
	QSettings settings("fairytale");
	settings.beginWriteArray("clipPackages", this->m_clipPackages.size());

	for (int i = 0; i < this->m_clipPackages.size(); ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("filePath", this->clipPackages()[i]->filePath());
	}

	settings.endArray();
}

void fairytale::playFinalClip(int index)
{
	this->m_completeSolutionIndex = index;

	this->m_player->playVideo(this, this->m_completeSolution[index]->narratorVideoUrl(), this->description(index, this->m_completeSolution[index]));
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
	this->clearAll();
	this->setGameButtonsEnabled(false);
	this->timeLabel->setText("");
	this->descriptionLabel->setText("");
	QMessageBox::information(this, tr("Game over!"), tr("GAME OVER!"));
}

void fairytale::nextTurn()
{
	this->timeLabel->setText(tr(""));
	this->descriptionLabel->setText("");

	this->clearClipButtons();

	bool won = false;

	if (!this->m_clips.empty())
	{
		this->fillCurrentClips();

		if (!this->m_currentClips.empty())
		{
			this->selectRandomSolution();

			if (this->m_turns == 0)
			{
				this->m_startPerson = this->m_currentSolution;
			}

			qDebug() << "Complete size " << m_clips.size();

			const QString description = this->description(this->m_turns, this->m_currentSolution);

			this->m_turns++;

			/*
			 * Play the narrator clip for the current solution as hint.
			 */
			this->m_player->playVideo(this, this->m_currentSolution->narratorVideoUrl(), description);
		}
		else
		{
			won = true;
		}
	}
	else
	{
		won = true;
	}

	if (won)
	{
		this->m_isRunning = false;
		QMessageBox::information(this, tr("WIN!"), tr("You won the game!!!!"), QMessageBox::Ok);

		if (this->customFairytaleDialog()->exec() == QDialog::Rejected)
		{
			this->clearAll();
			this->setGameButtonsEnabled(false);
		}
	}
}

void fairytale::clearClipButtons()
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
	this->m_completeSolutionIndex = 0;
	this->m_completeSolution.clear();

	foreach (QPushButton *button, this->m_completeSolutionButtons)
	{
		delete button;
	}

	this->m_completeSolutionButtons.clear();
	this->customFairytaleDialog()->clear();
}

void fairytale::clearClips()
{
	foreach (Clip *clip, this->m_clips)
	{
		delete clip;
	}

	this->m_clips.clear();
}

void fairytale::clearAll()
{
	clearClipButtons();
	clearSolution();
	clearClips();
}

void fairytale::setGameButtonsEnabled(bool enabled)
{
	actionPauseGame->setEnabled(enabled);
	actionShowCustomFairytale->setEnabled(enabled);
}

void fairytale::finishNarrator(QMediaPlayer::State state)
{
	if (state == QMediaPlayer::StoppedState)
	{
		if (!this->m_playCompleteSolution)
		{
			this->m_player->mediaPlayer()->stop();
			this->m_remainingTime = 1000 * (this->m_clips.size() + 1);
			this->updateTimeLabel();
			// the description label helps to remember
			this->descriptionLabel->setText(this->description(this->m_turns - 1, this->m_currentSolution));

			for (int i = 0; i < this->m_currentClips.size(); ++i)
			{
				this->m_buttons[i]->setIcon(QIcon(this->m_currentClips[i]->imageUrl().toLocalFile()));
				this->m_buttons[i]->setEnabled(true);
			}

			// run every second
			this->m_timer.start(1000);
		}
		else if (this->m_playCompleteSolution && this->m_completeSolutionIndex + 1 < this->m_completeSolution.size())
		{
			qDebug() << "Play next final clip";
			this->playFinalClip(this->m_completeSolutionIndex + 1);
		}
		/*
		 * Stop playing the complete solution.
		 */
		else if (this->m_playCompleteSolution)
		{
			qDebug() << "Finished final clips";
			this->m_playCompleteSolution = false;
			this->m_completeSolutionIndex = 0;
			//QMessageBox::information(this, tr("Finish"), tr("Finish"));
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
		if (QObject::sender() == this->m_buttons[i] && this->m_currentSolution == this->m_currentClips[i]) {
			addCurrentSolution();
			nextTurn();

			success = true;
		}
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
	this->customFairytaleDialog()->addClip(this->m_currentSolution);
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

	if (m_turns > 0)
	{
		m_requiresPerson = !m_requiresPerson;
	}

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

QString fairytale::description(int turn, Clip *clip)
{
	QString description;

	if (clip->isPerson())
	{
		if (turn == 0)
		{
			description = tr("<b>%1</b>").arg(clip->description());
		}
		else if (turn == 1)
		{
			description = tr("and <b>%1</b>").arg(clip->description());
		}
		else
		{
			description = tr("%1 and <b>%2</b>").arg(this->m_startPerson->description()).arg(clip->description());
		}
	}
	else
	{
		description = tr("<b>%1</b>").arg(clip->description());
	}

	return description;
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

CustomFairytaleDialog* fairytale::customFairytaleDialog()
{
	if (this->m_customFairytaleDialog == nullptr)
	{
		this->m_customFairytaleDialog = new CustomFairytaleDialog(this, this);
	}

	return this->m_customFairytaleDialog;
}

#include "fairytale.moc"
