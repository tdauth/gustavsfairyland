#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "clipeditor.h"
#include "languagedialog.h"
#include "recorder.h"
#include "fairytale.h"
#include "player.h"
#include "clip.h"

#include "clipeditor.moc"

void ClipEditor::clipIdChanged(const QString &text)
{
	this->m_clip->setId(text);
	checkForValidFields();
}

void ClipEditor::setIsPerson(bool isAPerson)
{
	qDebug() << "Toggle is person to" << isAPerson;
	this->m_clip->setIsPerson(isAPerson);
	checkForValidFields();
}

void ClipEditor::chooseImage()
{
	const QString filePath = QFileDialog::getOpenFileName(this, tr("Choose Image"), this->m_dir,  tr("All files (*);;Images (*.jpg *.jpeg *.png)"));

	if (!filePath.isEmpty())
	{
		const QUrl url = QUrl::fromLocalFile(filePath);
		this->m_clip->setImageUrl(url);
		QPixmap pixmap(filePath);
		this->imageLabel->setPixmap(pixmap.scaled(256, 256, Qt::KeepAspectRatio));
		this->imagePathLabel->setText(this->m_clip->imageUrl().toString());

		checkForValidFields();
	}
}

int ClipEditor::captureImage()
{
	const QFileInfo fileInfo = clipsDirectory().filePath("image");
	qDebug() << "Recording to file" << fileInfo.absoluteFilePath();
	m_recorder->setOutputFile(fileInfo.absoluteFilePath());
	const int result = m_recorder->showCameraFinder(QCamera::CaptureStillImage);

	if (result == QDialog::Accepted)
	{
		updateClipImage();
	}
	else
	{
		deleteRecordedFile();
	}

	qDebug() << "capture error state" << m_recorder->imageCapture()->error();

	return result;
}

void ClipEditor::showImage()
{
	m_player->showImage(m_app, m_clip->imageUrl(), m_clip->description());
}

void ClipEditor::imageSaved(int id, const QString &fileName)
{
	qDebug() << "Image saved as " << fileName;

	// Dont delete the current file.
	if (fileName != m_recordedFile)
	{
		deleteRecordedFile();
	}

	m_recordedFile = fileName;
	qDebug() << "Recorded file is" << m_recordedFile;

	updateClipImage();
}

void ClipEditor::chooseVideo()
{
#ifdef Q_OS_WIN
	const QString videoFormat = tr("msmpeg4v2 - WMV Files");
#else
	const QString videoFormat = tr("H264 compressed videos - MP4, OGG, MKV files with this codec");
#endif

	QMessageBox::information(this, tr("Video Format"), tr("Note that only some Video formats work on different platforms. Recommended video format for this platform: %1").arg(videoFormat));

	const QString filePath = QFileDialog::getOpenFileName(this, tr("Choose Video"), this->m_dir,  tr("All files (*);;Videos (*.mkv *.avi *.mp4 *.wmv)"));

	if (!filePath.isEmpty())
	{
		const QUrl url = QUrl::fromLocalFile(filePath);
		this->m_clip->setVideoUrl(url);
		// TODO set frame from video into label
		this->videoLabel->setText(this->m_clip->videoUrl().toString());

		checkForValidFields();
	}
}

int ClipEditor::recordVideo()
{
	const QFileInfo fileInfo = clipsDirectory().filePath("video");
	qDebug() << "Recording to file" << fileInfo.absoluteFilePath();
	m_recorder->setOutputFile(fileInfo.absolutePath());
	const int result = m_recorder->showCameraFinder(QCamera::CaptureVideo, true);

	if (result == QDialog::Accepted)
	{
		qDebug() << "Accepted";
		updateClipVideo();

		if (m_recorder->recorder()->error() != QMediaRecorder::NoError)
		{
			QMessageBox::critical(this, tr("Error"), tr("Error code %1 on recording a video.").arg(m_recorder->recorder()->error()));
		}
	}
	else
	{
		qDebug() << "Rejected, delete.";
		deleteRecordedFile();
	}

	qDebug() << "capture error state" << m_recorder->imageCapture()->error();

	return result;
}

void ClipEditor::playVideo()
{
	m_player->playVideo(m_app, m_clip->videoUrl(), m_clip->description(), false, false);
}

void ClipEditor::videoRecorderStateChanged(QMediaRecorder::State state)
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

void ClipEditor::audioRecorderStateChanged(QMediaRecorder::State state)
{
	qDebug() << "Audio recorder state changed" << state;

	if (state == QMediaRecorder::StoppedState)
	{
		const QString filePath = m_recorder->audioRecorder()->outputLocation().toLocalFile();
		qDebug() << "Saved audio file" << filePath;

		// Dont delete the current file.
		if (filePath != m_recordedFile)
		{
			deleteRecordedFile();
		}

		m_recordedFile = filePath;

		updateClipNarratingSound();
	}
}

void ClipEditor::updateClipImage()
{
	// Use temporary file.
	const QString fileName = m_recordedFile;

	if (fileName.isEmpty() || m_recorder->result() != QDialog::Accepted)
	{
		return;
	}

	const QFileInfo fileInfo(m_recordedFile);

	qDebug() << "Using file" << m_recordedFile << "with size" << fileInfo.size() << "exists:" << fileInfo.exists() << " and capture error state" << m_recorder->imageCapture()->error();

	if (fileInfo.exists())
	{
		const QPixmap pixmap(fileName);
		qDebug() << "Pixmap:" << pixmap.size();
		imageLabel->setPixmap(pixmap.scaled(256, 256, Qt::KeepAspectRatio));
		imagePathLabel->setText(fileName);
		m_clip->setImageUrl(QUrl::fromLocalFile(fileName));

		// Dont delete the old file. TODO leaks afterwards when removed.
		m_recordedFile.clear();

		checkForValidFields();
	}
}

void ClipEditor::updateClipVideo()
{
	qDebug() << "Updating video clip";

	// Use temporary file.
	const QString fileName = m_recordedFile;

	/*
	 * The recorder dialog has to be accepted, otherwise the file is recorded but not used for the clip.
	 */
	if (fileName.isEmpty() || m_recorder->result() != QDialog::Accepted)
	{
		qDebug() << "File does not exist" << fileName << "or result is not accepted" << m_recorder->result();

		return;
	}

	const QFileInfo fileInfo(fileName);
	qDebug() << "Using file" << fileName << "with size" << fileInfo.size() << "exists:" << fileInfo.exists() << " and capture error state" << m_recorder->recorder()->error();

	if (fileInfo.exists())
	{
		m_clip->setVideoUrl(QUrl::fromLocalFile(fileName));
		videoLabel->setText(fileName);

		// Dont delete the old file. TODO leaks afterwards when removed.
		m_recordedFile.clear();

		checkForValidFields();
	}
}

void ClipEditor::updateClipNarratingSound()
{
	// Use temporary file.
	const QString fileName = m_recordedFile;

	if (fileName.isEmpty() || m_recorder->result() != QDialog::Accepted)
	{
		return;
	}

	const QFileInfo fileInfo(fileName);
	qDebug() << "Using file" << fileName << "with size" << fileInfo.size() << "exists:" << fileInfo.exists() << " and capture error state" << m_recorder->audioRecorder()->error();

	if (fileInfo.exists())
	{
		if (!m_language.isEmpty())
		{
			Clip::Urls narratorUrls = this->m_clip->narratorUrls();
			narratorUrls.insert(m_language, QUrl::fromLocalFile(fileName));
			this->m_clip->setNarratorUrls(narratorUrls);

			updateNarratingSoundsWidget(m_clip);

			// Dont delete the old file. TODO leaks afterwards when removed.
			m_recordedFile.clear();

			checkForValidFields();
		}
	}
}

void ClipEditor::updateClipNarratingSoundForAllLanguages()
{
	// Use temporary file.
	const QString fileName = m_recordedFile;

	if (fileName.isEmpty() || m_recorder->result() != QDialog::Accepted)
	{
		return;
	}

	const QFileInfo fileInfo(fileName);
	qDebug() << "Using file" << fileName << "with size" << fileInfo.size() << "exists:" << fileInfo.exists() << " and capture error state" << m_recorder->audioRecorder()->error();

	if (fileInfo.exists())
	{
		Clip::Urls narratorUrls = this->m_clip->narratorUrls();
		// TODO all languages
		narratorUrls.insert("de", QUrl::fromLocalFile(fileName));
		narratorUrls.insert("en", QUrl::fromLocalFile(fileName));
		this->m_clip->setNarratorUrls(narratorUrls);

		updateNarratingSoundsWidget(m_clip);

		// Dont delete the old file. TODO leaks afterwards when removed.
		m_recordedFile.clear();

		checkForValidFields();
	}
}

void ClipEditor::updateAllDescriptions(const QString& description)
{
	Clip::Descriptions descriptions = this->m_clip->descriptions();
	// TODO all languages
	descriptions.insert("de", description);
	descriptions.insert("en", description);
	this->m_clip->setDescriptions(descriptions);

	updateDescriptionWidget(m_clip);

	checkForValidFields();
}

void ClipEditor::onFinish(int result)
{
	if (result == QDialog::Accepted && checkForValidFields())
	{
		moveFilesToCurrentClipIdDir(targetClipsDirectory(), this->m_clip->id());
	}
	// TODO delete temporary stuff?!
	else
	{
	}
}

void ClipEditor::addNarratingSound()
{
	if (this->execLanguageDialog().isEmpty())
	{
		return;
	}

	const QString filePath = QFileDialog::getOpenFileName(this, tr("Choose Narrating Sound"), this->m_dir,  tr("All files (*);;Audio (*.wav)"));

	if (!filePath.isEmpty())
	{
		const QUrl url = QUrl::fromLocalFile(filePath);

		if (!m_language.isEmpty())
		{
			Clip::Urls narratorUrls = this->m_clip->narratorUrls();
			narratorUrls.insert(m_language, url);
			this->m_clip->setNarratorUrls(narratorUrls);

			updateNarratingSoundsWidget(m_clip);

			checkForValidFields();
		}
	}
}

void ClipEditor::recordNarratingSound()
{
	if (execLanguageDialog().isEmpty())
	{
		return;
	}

	const QFileInfo fileInfo = clipsDirectory().filePath("audio_" + m_language);
	qDebug() << "Recording to file" << fileInfo.absoluteFilePath();
	m_recorder->setOutputFile(fileInfo.absoluteFilePath());

	if (m_recorder->showAudioRecorder() == QDialog::Accepted)
	{
		updateClipNarratingSound();
	}
	else
	{
		deleteRecordedFile();
	}

	qDebug() << "capture error state" << m_recorder->imageCapture()->error();
}

int ClipEditor::recordNarratingSoundSimple()
{
	const QFileInfo fileInfo = clipsDirectory().filePath("audio_all");
	qDebug() << "Recording to file" << fileInfo.absoluteFilePath();
	m_recorder->setOutputFile(fileInfo.absoluteFilePath());
	const int result = m_recorder->showAudioRecorder(true);

	if (result == QDialog::Accepted)
	{
		updateClipNarratingSoundForAllLanguages();
	}
	else
	{
		deleteRecordedFile();
	}

	qDebug() << "capture error state" << m_recorder->imageCapture()->error();

	return result;
}

void ClipEditor::playNarratingSound()
{
	if (!narratingSoundsListWidget->selectedItems().isEmpty())
	{
		QListWidgetItem *item = narratingSoundsListWidget->selectedItems().front();
		const QString language = item->data(Qt::UserRole).toString();

		m_player->playSound(m_app, m_clip->narratorUrls()[language], m_clip->description(), QUrl(), false, false);
	}
}

void ClipEditor::playNarratingSoundSimple()
{
	m_player->playSound(m_app, m_clip->narratorUrl(), m_clip->description(), QUrl(), false, false);
}

void ClipEditor::removeNarratingSound()
{
	if (!narratingSoundsListWidget->selectedItems().isEmpty())
	{
		QListWidgetItem *item = narratingSoundsListWidget->selectedItems().front();
		const QString language = item->data(Qt::UserRole).toString();

		Clip::Urls narratorUrls = this->m_clip->narratorUrls();
		narratorUrls.remove(language);
		this->m_clip->setNarratorUrls(narratorUrls);

		updateNarratingSoundsWidget(m_clip);

		checkForValidFields();
	}
}

void ClipEditor::addDescription()
{
	if (this->execLanguageDialog().isEmpty())
	{
		return;
	}

	const QString description = QInputDialog::getText(this, tr("Description"), tr("Description:"));

	if (!description.isEmpty())
	{
		if (!m_language.isEmpty())
		{
			Clip::Descriptions descriptions = this->m_clip->descriptions();
			descriptions.insert(m_language, description);
			this->m_clip->setDescriptions(descriptions);

			updateDescriptionWidget(m_clip);

			checkForValidFields();
		}
	}
}

void ClipEditor::removeDescription()
{
	if (!descriptionsListWidget->selectedItems().isEmpty())
	{
		QListWidgetItem *item = descriptionsListWidget->selectedItems().front();
		const QString language = item->data(Qt::UserRole).toString();

		Clip::Descriptions descriptions = this->m_clip->descriptions();
		descriptions.remove(language);
		this->m_clip->setDescriptions(descriptions);

		updateDescriptionWidget(m_clip);

		checkForValidFields();
	}
}

QString ClipEditor::execLanguageDialog()
{
	if (this->m_languageDialog == nullptr)
	{
		this->m_languageDialog = new LanguageDialog(this);
	}

	if (this->m_languageDialog->exec() == QDialog::Accepted)
	{
		const QString result = this->m_languageDialog->getLanguage();
		m_language = result;

		return result;
	}

	return QString();
}

ClipEditor::ClipEditor(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app), m_clip(new Clip(m_app, this)), m_languageDialog(nullptr), m_recorder(new gustav::Recorder(this)), m_player(new Player(this, app))
{
	setupUi(this);

	this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false); // enable when all fields are valid

	connect(this->clipIdLineEdit, &QLineEdit::textChanged, this, &ClipEditor::clipIdChanged);
	connect(this->isAPersonCheckBox, &QCheckBox::toggled, this, &ClipEditor::setIsPerson);
	connect(this->imagePushButton, &QPushButton::clicked, this, &ClipEditor::chooseImage);
	connect(this->captureImagePushButton, &QPushButton::clicked, this, &ClipEditor::captureImage);
	connect(this->showImageButton, &QPushButton::clicked, this, &ClipEditor::showImage);
	connect(this->videoPushButton, &QPushButton::clicked, this, &ClipEditor::chooseVideo);
	connect(this->recordVideoPushButton, &QPushButton::clicked, this, &ClipEditor::recordVideo);
	connect(this->playVideoPushButton, &QPushButton::clicked, this, &ClipEditor::playVideo);

	connect(this->addNarratingSoundPushButton, &QPushButton::clicked, this, &ClipEditor::addNarratingSound);
	connect(this->recordNarratingSoundPushButton, &QPushButton::clicked, this, &ClipEditor::recordNarratingSound);
	connect(this->simpleRecordNarratingSoundPushButton, &QPushButton::clicked, this, &ClipEditor::recordNarratingSoundSimple);
	connect(this->playNarratingSoundPushButton, &QPushButton::clicked, this, &ClipEditor::playNarratingSound);
	connect(this->simplePlayNarratingSoundPushButton, &QPushButton::clicked, this, &ClipEditor::playNarratingSoundSimple);
	connect(this->removeNarratingSoundPushButton, &QPushButton::clicked, this, &ClipEditor::removeNarratingSound);
	connect(this->addDescriptionPushButton, &QPushButton::clicked, this, &ClipEditor::addDescription);
	connect(this->removeDescriptionPushButton, &QPushButton::clicked, this, &ClipEditor::removeDescription);

	connect(this->descriptionLineEdit, &QLineEdit::textChanged, this, &ClipEditor::updateAllDescriptions);

	connect(this->buttonBox, &QDialogButtonBox::accepted, this, &ClipEditor::accept);
	connect(this->buttonBox, &QDialogButtonBox::rejected, this, &ClipEditor::reject);
	connect(this, &QDialog::finished, this, &ClipEditor::onFinish);

	connect(this->m_recorder->imageCapture(), &QCameraImageCapture::imageSaved, this, &ClipEditor::imageSaved);
	connect(this->m_recorder->recorder(), &QMediaRecorder::stateChanged, this, &ClipEditor::videoRecorderStateChanged);
	connect(this->m_recorder->audioRecorder(), &QMediaRecorder::stateChanged, this, &ClipEditor::audioRecorderStateChanged);

	this->advancedGroupBox->setChecked(false);
	this->advancedWidget->hide();

	QSettings settings("fairytale");
	m_dir = settings.value("clipeditordir").toString();

	// Default target dir:
	m_targetClipsDirectory = clipsDirectory();
}

ClipEditor::~ClipEditor()
{
	QSettings settings("fairytale");
	settings.setValue("clipeditordir", m_dir);
}

void ClipEditor::fill(Clip *clip)
{
	this->clipIdLineEdit->setText(clip->id());
	this->isAPersonCheckBox->setChecked(clip->isPerson());
	QPixmap pixmap(clip->imageUrl().toLocalFile());
	this->imageLabel->setPixmap(pixmap.scaled(256, 256, Qt::KeepAspectRatio));
	this->imagePathLabel->setText(clip->imageUrl().toString());
	this->videoLabel->setText(clip->videoUrl().toString());

	updateNarratingSoundsWidget(clip);
	updateDescriptionWidget(clip);

	this->m_clip->assign(*clip);

	this->checkForValidFields();
}

void ClipEditor::assignToClip(Clip *clip)
{
	clip->assign(*this->m_clip);
	qDebug() << "Assigned clip is person" << m_clip->isPerson();
}

Clip* ClipEditor::clip(QObject *parent)
{
	qDebug() << "Clips description on getting clip" << m_clip->description();
	qDebug() << "clip is person" << m_clip->isPerson();

	return new Clip(*m_clip, parent);
}

bool ClipEditor::checkForValidFields()
{
	const bool result = !this->m_clip->id().isEmpty() && !this->m_clip->descriptions().isEmpty() && !this->m_clip->imageUrl().isEmpty() && !this->m_clip->videoUrl().isEmpty() && !this->m_clip->narratorUrls().isEmpty();

	// TODO make sure the ID is unique
	this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(result);

	showImageButton->setEnabled(!this->m_clip->imageUrl().isEmpty());
	playVideoPushButton->setEnabled(!this->m_clip->videoUrl().isEmpty());
	simplePlayNarratingSoundPushButton->setEnabled(!this->m_clip->narratorUrl().isEmpty());

	return result;
}

bool ClipEditor::deleteRecordedFile()
{
	// delete old temporary video file
	if (!m_recordedFile.isEmpty())
	{
		QFile file(m_recordedFile);

		return file.remove();
	}

	return false;
}

QDir ClipEditor::clipsDirectory() const
{
	const QDir clipsDir = QDir(this->m_app->customClipsDirectory());
	const QFileInfo subDir = clipsDir.filePath("tmp");

	if (!subDir.exists())
	{
		clipsDir.mkdir("tmp");
	}

	return QDir(subDir.absoluteFilePath());
}

QDir ClipEditor::currentClipDirectory() const
{
	return QDir(clipsDirectory().filePath(this->m_clip->id()));
}

bool ClipEditor::clipIdIsAlreadyUsed() const
{
	const QFileInfo fileInfo(clipsDirectory().filePath(this->m_clip->id()));

	return fileInfo.exists();
}

bool ClipEditor::moveFileToCurrentClipDir(const QDir parentDir, const QUrl &oldUrl, QUrl &newUrl)
{
	const QString oldImageFilePath = this->m_app->resolveClipUrl(oldUrl).toLocalFile();
	const QFileInfo oldImageFile(oldImageFilePath);
	bool result = oldImageFile.exists();

	if (result)
	{
		qDebug() << "New clips dir still is" << parentDir;
		qDebug() << "image file name is" << oldImageFile.fileName();
		const QString target = parentDir.filePath(oldImageFile.fileName());

		qDebug() << "Has old file and copying it to " << target;

		if (!QFile::copy(oldImageFile.absoluteFilePath(), target))
		{
			qDebug() << "Copying failed";

			result = false;
		}
		/*
		 * Make sure to use relative paths which work everywhere.
		 */
		else
		{
			newUrl = QUrl::fromLocalFile(target);
			qDebug() << "New image URL" << newUrl;

			/*
			 * Delete the old file to safe space.
			 * But only if it is a temporary recorded file.
			 */
			if (oldImageFile.dir() == clipsDirectory())
			{
				QFile::remove(oldImageFile.absoluteFilePath());
			}
		}
	}
	else
	{
		qDebug() << "File " << oldImageFile.absoluteFilePath() << " does not even exist.";
	}

	return result;
}

bool ClipEditor::moveFilesToCurrentClipIdDir(const QDir parentDir, const QString dirName)
{
	qDebug() << "New clips dir" << parentDir;
	const QDir newDir = parentDir.filePath(dirName);

	if (!newDir.exists())
	{
		if (!parentDir.mkdir(dirName))
		{
			qDebug() << "Error on creating sub directory" << dirName;

			return false;
		}
	}

	bool result = true;
	QUrl newImageUrl;

	if (moveFileToCurrentClipDir(newDir, this->m_clip->imageUrl(), newImageUrl))
	{
		m_clip->setImageUrl(newImageUrl);
	}
	else
	{
		result = false;
	}

	QUrl newVideoUrl;

	if (moveFileToCurrentClipDir(newDir, this->m_clip->videoUrl(), newVideoUrl))
	{
		m_clip->setVideoUrl(newVideoUrl);
	}
	else
	{
		result = false;
	}

	Clip::Urls narratorUrls = this->m_clip->narratorUrls();
	Clip::Urls newNarratorUrls;

	foreach (const QString language, narratorUrls.keys())
	{
		const QUrl url = narratorUrls[language];
		QUrl newSoundUrl;

		if (moveFileToCurrentClipDir(newDir, url, newSoundUrl))
		{
			newNarratorUrls.insert(language, newSoundUrl);
		}
		else
		{
			result = false;
			newNarratorUrls.insert(language, url);
		}
	}

	this->m_clip->setNarratorUrls(newNarratorUrls);

	/*
	 * Update the whole GUI again after changing paths.
	 */
	this->fill(this->m_clip);

	return result;
}

void ClipEditor::updateNarratingSoundsWidget(Clip* clip)
{
	this->narratingSoundsListWidget->clear();

	for (Clip::Urls::const_iterator iterator = clip->narratorUrls().begin(); iterator != clip->narratorUrls().end(); ++iterator)
	{
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(iterator.value().toString());
		item->setData(Qt::UserRole, iterator.key());
		this->narratingSoundsListWidget->addItem(item);
	}
}

void ClipEditor::updateDescriptionWidget(Clip *clip)
{
	this->descriptionsListWidget->clear();

	for (Clip::Descriptions::const_iterator iterator = clip->descriptions().begin(); iterator != clip->descriptions().end(); ++iterator)
	{
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(iterator.value());
		item->setData(Qt::UserRole, iterator.key());
		this->descriptionsListWidget->addItem(item);
	}
}

void ClipEditor::setIsNewClip(bool isNewClip)
{
	if (isNewClip)
	{
		titleLabel->setText(tr("Add new clip"));
	}
	else
	{
		titleLabel->setText(tr("Edit clip"));
	}
}
