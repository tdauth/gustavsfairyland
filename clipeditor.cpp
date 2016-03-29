#include <iostream>

#include <QFileDialog>
#include <QSettings>

#include "clipeditor.h"

#include "clipeditor.moc"

#include "clip.h"

void ClipEditor::descriptionChanged(const QString& description)
{
	this->m_clip->setDescription(description);
	checkForValidFields();

	std::cerr << "New description: " << description.toUtf8().constData() << std::endl;
}

void ClipEditor::setIsPerson(bool isAPerson)
{
	this->m_clip->setIsPerson(isAPerson);
	checkForValidFields();
}

void ClipEditor::chooseImage()
{
	const QString filePath = QFileDialog::getOpenFileName(this, tr("Choose Image"), this->m_dir,  tr("JPEG (*.jpg *.jpeg);;PNG (*.png)"));

	if (!filePath.isEmpty())
	{
		const QUrl url = QUrl::fromLocalFile(filePath);
		this->m_clip->setImageUrl(url);
		QPixmap pixmap(filePath);
		this->imageLabel->setPixmap(pixmap.scaled(64, 64));

		checkForValidFields();
	}
}

void ClipEditor::chooseVideo()
{
	const QString filePath = QFileDialog::getOpenFileName(this, tr("Choose Video"), this->m_dir,  tr("MKV (*.mkv);;AVI (*.avi);;MP4 (*.mp4)"));

	if (!filePath.isEmpty())
	{
		const QUrl url = QUrl::fromLocalFile(filePath);
		this->m_clip->setVideoUrl(url);
		// TODO set frame from video into label

		checkForValidFields();
	}
}

void ClipEditor::chooseNarratorVideo()
{
	const QString filePath = QFileDialog::getOpenFileName(this, tr("Choose Narrator Video"), this->m_dir,  tr("MKV (*.mkv);;AVI (*.avi);;MP4 (*.mp4)"));

	if (!filePath.isEmpty())
	{
		const QUrl url = QUrl::fromLocalFile(filePath);
		this->m_clip->setNarratorVideoUrl(url);
		// TODO set frame from video into label

		checkForValidFields();
	}
}

ClipEditor::ClipEditor(QWidget* parent) : QDialog(parent), m_clip(new Clip(this))
{
	setupUi(this);

	this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false); // enable when all fields are valid

	connect(this->descriptionLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(descriptionChanged(const QString&)));
	connect(this->isAPersonCheckBox, SIGNAL(toggled(bool)), this, SLOT(setIsPerson(bool)));

	connect(this->imagePushButton, &QPushButton::clicked, this, &ClipEditor::chooseImage);
	connect(this->videoPushButton, &QPushButton::clicked, this, &ClipEditor::chooseVideo);
	connect(this->narratorVideoPushButton, &QPushButton::clicked, this, &ClipEditor::chooseNarratorVideo);

	connect(this->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(this->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QSettings settings("fairytale");
	m_dir = settings.value("clipeditordir").toString();
}

ClipEditor::~ClipEditor()
{
	QSettings settings("fairytale");
	settings.setValue("clipeditordir", m_dir);
}

Clip* ClipEditor::clip(QObject *parent)
{
	std::cerr << "Clips description on getting clip " << m_clip->description().toUtf8().constData() << std::endl;

	return new Clip(*m_clip, parent);
}

bool ClipEditor::checkForValidFields()
{
	const bool result = !this->m_clip->description().isEmpty() && !this->m_clip->imageUrl().isEmpty() && !this->m_clip->videoUrl().isEmpty() && !this->m_clip->narratorVideoUrl().isEmpty();

	this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(result);

	return result;
}
