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

	m_continueButton = new QPushButton(tr("Continue"), app);
	connect(m_continueButton, &QPushButton::clicked, this, &GameModeStory::continueGameMode);
	app->gameAreaLayout()->addWidget(m_continueButton);
	m_continueButton->hide();

	m_finishButton = new QPushButton(tr("Complete"), app);
	connect(m_finishButton, &QPushButton::clicked, this, &GameModeStory::finishGameMode);
	app->gameAreaLayout()->addWidget(m_finishButton);
	m_finishButton->hide();
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
	m_continueButton->hide();
	m_finishButton->hide();
	m_recorder->hide();
}

void GameModeStory::start()
{
	m_continueButton->show();
	m_finishButton->show();
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

		m_recorder->showCameraFinder(QCamera::CaptureVideo);

		m_recorder->showCameraFinder(QCamera::CaptureStillImage);

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
