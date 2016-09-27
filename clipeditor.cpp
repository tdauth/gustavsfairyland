#include <iostream>

#include <QSettings>
#include <QtWidgets/QtWidgets>

#include "clipeditor.h"
#include "languagedialog.h"

#include "clipeditor.moc"

#include "clip.h"

void ClipEditor::clipIdChanged(const QString &text)
{
	this->m_clip->setId(text);
	checkForValidFields();
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

void ClipEditor::addNarratingSound()
{
	const QString filePath = QFileDialog::getOpenFileName(this, tr("Choose Narrating Sound"), this->m_dir,  tr("WAV (*.wav)"));

	if (!filePath.isEmpty())
	{
		const QUrl url = QUrl::fromLocalFile(filePath);
		const QString language = this->execLanguageDialog();

		if (!language.isEmpty())
		{
			Clip::Urls narratorUrls = this->m_clip->narratorUrls();
			narratorUrls.insert(language, url);
			this->m_clip->setNarratorUrls(narratorUrls);
			this->narratingSoundsListWidget->addItem(filePath);

			checkForValidFields();
		}
	}
}

void ClipEditor::addDescription()
{
	const QString description = QInputDialog::getText(this, tr("Description"), tr("Description:"));

	if (!description.isEmpty())
	{
		const QString language = this->execLanguageDialog();

		if (!language.isEmpty())
		{
			Clip::Descriptions descriptions = this->m_clip->descriptions();
			descriptions.insert(language, description);
			this->m_clip->setDescriptions(descriptions);
			this->descriptionsListWidget->addItem(description);

			checkForValidFields();
		}
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
		return this->m_languageDialog->getLanguage();
	}

	return QString();
}

ClipEditor::ClipEditor(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app), m_clip(new Clip(m_app, this)), m_languageDialog(nullptr)
{
	setupUi(this);

	this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false); // enable when all fields are valid

	connect(this->clipIdLineEdit, &QLineEdit::textChanged, this, &ClipEditor::clipIdChanged);
	connect(this->isAPersonCheckBox, SIGNAL(toggled(bool)), this, SLOT(setIsPerson(bool)));
	connect(this->imagePushButton, &QPushButton::clicked, this, &ClipEditor::chooseImage);
	connect(this->videoPushButton, &QPushButton::clicked, this, &ClipEditor::chooseVideo);

	connect(this->addNarratingSoundPushButton, &QPushButton::clicked, this, &ClipEditor::addNarratingSound);
	connect(this->addDescriptionPushButton, &QPushButton::clicked, this, &ClipEditor::addDescription);

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

void ClipEditor::fill(Clip *clip)
{
	this->clipIdLineEdit->setText(clip->id());
	this->isAPersonCheckBox->setChecked(clip->isPerson());
	QPixmap pixmap(clip->imageUrl().toLocalFile());
	this->imageLabel->setPixmap(pixmap.scaled(64, 64));

	this->narratingSoundsListWidget->clear();

	for (Clip::Urls::const_iterator iterator = clip->narratorUrls().begin(); iterator != clip->narratorUrls().end(); ++iterator)
	{
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(iterator.value().toString());
		item->setData(Qt::UserRole, iterator.key());
		this->narratingSoundsListWidget->addItem(item);
	}

	this->descriptionsListWidget->clear();

	for (Clip::Descriptions::const_iterator iterator = clip->descriptions().begin(); iterator != clip->descriptions().end(); ++iterator)
	{
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(iterator.value());
		item->setData(Qt::UserRole, iterator.key());
		this->descriptionsListWidget->addItem(item);
	}

	this->m_clip->assign(*clip);

	this->checkForValidFields();
}

void ClipEditor::assignToClip(Clip *clip)
{
	clip->assign(*this->m_clip);
}

Clip* ClipEditor::clip(QObject *parent)
{
	std::cerr << "Clips description on getting clip " << m_clip->description().toUtf8().constData() << std::endl;

	return new Clip(*m_clip, parent);
}

bool ClipEditor::checkForValidFields()
{
	const bool result = !this->m_clip->id().isEmpty() && !this->m_clip->descriptions().isEmpty() && !this->m_clip->imageUrl().isEmpty() && !this->m_clip->videoUrl().isEmpty() && !this->m_clip->narratorUrls().isEmpty();

	// TODO make sure the ID is unique
	this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(result);

	return result;
}
