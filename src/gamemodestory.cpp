#include <QtGui>
#include <QtWidgets>
#include <QtNetwork>

#include "gamemodestory.h"
#include "fairytale.h"
#include "clippackage.h"
#include "clip.h"

GameModeStory::GameModeStory(fairytale *app) : GameMode(app), m_state(State::None), m_currentSolution(nullptr), m_recorder(new Recorder(app)), m_networkAccessManager(new QNetworkAccessManager(this))
{
	connect(m_recorder->recorder(), &QMediaRecorder::stateChanged, this, &GameModeStory::onVideoRecorderStateChanged);
	connect(m_recorder->imageCapture(), &QCameraImageCapture::imageSaved, this, &GameModeStory::onImageCaptured);
	connect(m_recorder->audioRecorder(), &QMediaRecorder::stateChanged, this, &GameModeStory::onAudioRecorderStateChanged);

	m_clipsWidget = new QWidget(app);
	m_clipsWidget->setLayout(new QGridLayout());
	app->gameAreaLayout()->addWidget(m_clipsWidget);
	m_clipsWidget->hide();
	m_clipsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_buttonsWidget = new QWidget(app);
	QHBoxLayout *layout = new QHBoxLayout();
	m_buttonsWidget->setLayout(layout);
	app->gameAreaLayout()->addWidget(m_buttonsWidget);
	m_buttonsWidget->hide();

	//layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Minimum));

	m_continueButton = new QPushButton(tr("Continue"), m_buttonsWidget);
	connect(m_continueButton, &QPushButton::clicked, this, &GameModeStory::continueGameMode);
	m_buttonsWidget->layout()->addWidget(m_continueButton);

	m_finishButton = new QPushButton(tr("Complete"), m_buttonsWidget);
	connect(m_finishButton, &QPushButton::clicked, this, &GameModeStory::finishGameMode);
	m_buttonsWidget->layout()->addWidget(m_finishButton);

	//layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Minimum));
}

GameMode::State GameModeStory::state()
{
	return this->m_state;
}

Clip* GameModeStory::solution()
{
	return this->m_currentSolution;
}

long int GameModeStory::time()
{
	return 0;
}

void GameModeStory::afterNarrator()
{
}

void GameModeStory::nextTurn()
{
	this->m_currentSolution = nullptr;


	setState(State::Running);
}

void GameModeStory::resume()
{
}

void GameModeStory::pause()
{
}

void GameModeStory::end()
{
	this->m_currentSolution = nullptr;
	m_clipsWidget->hide();
	m_buttonsWidget->hide();
	m_recorder->hide();
}

void GameModeStory::start()
{
	m_clipsWidget->show();
	m_buttonsWidget->show();
	continueGameMode();
}

QString GameModeStory::name() const
{
	return tr("Writing Stories");
}

QString GameModeStory::id() const
{
	return "story";
}

void GameModeStory::setState(State state)
{
	this->m_state = state;
}

bool GameModeStory::hasToChooseTheSolution()
{
	return false;
}

bool GameModeStory::hasLimitedTime()
{
	return false;
}

bool GameModeStory::playIntro()
{
	return false;
}

bool GameModeStory::playOutro()
{
	return false;
}

bool GameModeStory::useMaxRounds()
{
	return false;
}

bool GameModeStory::useDifficulty()
{
	return false;
}

bool GameModeStory::showWinDialog()
{
	return false;
}

bool GameModeStory::unlockBonusClip()
{
	return false;
}

bool GameModeStory::addToHighScores()
{
	return false;
}

void GameModeStory::onVideoRecorderStateChanged(QMediaRecorder::State state)
{
	qDebug() << "Recorder state changed" << state;

	if (state == QMediaRecorder::StoppedState)
	{
		qDebug() << "Saved file";

		const QString filePath = m_recorder->recorder()->outputLocation().toLocalFile();

		// Dont delete the current file.
		if (filePath != m_recordedFile)
		{
			deleteRecordedFile();
		}

		m_recordedFile = filePath;

		updateClipVideo();
	}
}

void GameModeStory::onAudioRecorderStateChanged(QMediaRecorder::State state)
{
}

void GameModeStory::onImageCaptured(int id, const QString &fileName)
{
}

void GameModeStory::continueGameMode()
{
	qDebug() << "After narrator game mode story";

	if (networkAccessManager()->networkAccessible() == QNetworkAccessManager::Accessible)
	{
		QNetworkRequest request;
		request.setUrl(QUrl("http://wc3lib.org/getlist"));
		request.setRawHeader("User-Agent", "gustavsfairyland");
		QNetworkReply *reply = networkAccessManager()->get(request);

		QEventLoop loop;
		connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		loop.exec();

		qDebug() << "Answer:" << reply->readAll();

		// TODO check for canceling
		const QFileInfo fileInfo = clipsDirectory().filePath("video");
		qDebug() << "Recording to file" << fileInfo.absoluteFilePath();
		m_recorder->setOutputFile(fileInfo.absolutePath());

		if (m_recorder->showCameraFinder(QCamera::CaptureVideo) == QDialog::Accepted)
		{
			updateClipVideo();

			if (m_recorder->showCameraFinder(QCamera::CaptureStillImage) == QDialog::Accepted)
			{
			}
		}
	}
	else
	{
		QMessageBox::warning(this->app(), tr("No network!"), tr("Network is not available."));

		setState(State::Lost);
	}

	qDebug() << "Finish turn";

	this->app()->onFinishTurn();
}

void GameModeStory::finishGameMode()
{
	setState(GameMode::State::Won);

	this->app()->onFinishTurn();
}

bool GameModeStory::deleteRecordedFile()
{
	// delete old temporary video file
	if (!m_recordedFile.isEmpty())
	{
		QFile file(m_recordedFile);

		return file.remove();
	}

	return false;
}

void GameModeStory::updateClipVideo()
{
	// Use temporary file.
	const QString fileName = m_recordedFile;

	/*
	 * The recorder dialog has to be accepted, otherwise the file is recorded but not used for the clip.
	 */
	if (fileName.isEmpty() || m_recorder->result() != QDialog::Accepted)
	{
		return;
	}

	const QFileInfo fileInfo(fileName);
	qDebug() << "Using file" << fileName << "with size" << fileInfo.size() << "exists:" << fileInfo.exists() << " and capture error state" << m_recorder->recorder()->error();

	if (fileInfo.exists())
	{
		QLabel *label = new QLabel(fileInfo.fileName());
		m_clipsWidget->layout()->addWidget(label);

		// Dont delete the old file. TODO leaks afterwards when removed.
		m_recordedFile.clear();
	}
}

QDir GameModeStory::clipsDirectory() const
{
	const QDir clipsDir = QDir(this->app()->clipsDir().toLocalFile());
	const QFileInfo subDir = clipsDir.filePath("tmpstorymode");

	if (!subDir.exists())
	{
		clipsDir.mkdir("tmpstorymode");
	}

	return QDir(subDir.absoluteFilePath());
}
