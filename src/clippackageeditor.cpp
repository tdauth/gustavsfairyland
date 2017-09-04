#include <QtWidgets>
#include <QtCore>

#include "clippackageeditor.h"
#include "clipeditor.h"
#include "clippackage.h"
#include "clip.h"
#include "bonusclip.h"

#include "clippackageeditor.moc"
#include "fairytale.h"

void ClipPackageEditor::idChanged(const QString &text)
{
	this->m_clipPackage->setId(text);

	checkForValidFields();
}

void ClipPackageEditor::loadCustomClipsPackage()
{
	if (m_app->customClipPackage() == nullptr)
	{
		QMessageBox::critical(this, tr("Error"), tr("Custom clip package does not exist. Try to restore default settings."));

		return;
	}

	const QFileInfo fileInfo(m_app->customClipPackage()->filePath());

	if (!fileInfo.exists() || !fileInfo.isReadable())
	{
		QMessageBox::critical(this, tr("Error"), tr("File %1 does not exist or is not readable.").arg(fileInfo.absoluteFilePath()));

		return;
	}

	this->m_clipPackage->clear();
	treeWidget->clear();
	bonusClipTreeWidget->clear();

	checkForValidFields();

	if (!this->m_clipPackage->loadClipsFromFile(fileInfo.absoluteFilePath()))
	{
		QMessageBox::critical(this, tr("Error"), tr("Error on loading clip description file."));

		return;
	}

	loadClipPackage();
}

void ClipPackageEditor::addClip()
{
	if (this->clipEditor()->exec() == QDialog::Accepted)
	{
		Clip *clip = this->clipEditor()->clip(m_clipPackage);
		m_clipPackage->addClip(clip);
		QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
		item->setData(0, Qt::UserRole, clip->id());
		item->setText(0, clip->description());
		treeWidget->addTopLevelItem(item);

		qDebug() << "Adding clip to tree widget with description " << clip->description();
	}
}

void ClipPackageEditor::editClip()
{
	if (!treeWidget->selectedItems().isEmpty())
	{
		QTreeWidgetItem *item = treeWidget->selectedItems().first();
		const QString id = item->data(0, Qt::UserRole).toString();
		Clip *clip = this->m_clipPackage->clips()[id];

		this->clipEditor()->setIsNewClip(false);
		this->clipEditor()->fill(clip);

		if (this->clipEditor()->exec() == QDialog::Accepted)
		{
			this->clipEditor()->assignToClip(clip);
		}
	}
	// TODO allow editting bonus clips
}

void ClipPackageEditor::removeClip()
{
	if (!treeWidget->selectedItems().isEmpty())
	{
		QTreeWidgetItem *item = treeWidget->selectedItems().first();
		const QString id = item->data(0, Qt::UserRole).toString();
		Clip *clip = this->m_clipPackage->clips()[id];
		this->m_clipPackage->clips().remove(id);

		delete clip;
		clip = nullptr;

		delete item;
		item = nullptr;
	}
}

void ClipPackageEditor::newPackage()
{
	if (QMessageBox::question(this, tr("Clear all clips?"), tr("Do you really want to clear all clips?")) == QMessageBox::Yes)
	{
		this->m_clipPackage->clear();
		treeWidget->clear();

		checkForValidFields();
	}
}

void ClipPackageEditor::loadPackage()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"), this->m_dir, tr("All Files (*);;Clip Description (*.xml);;Compressed Clip Package (*.pkgc)"));

	if (!fileName.isEmpty())
	{
		this->m_clipPackage->clear();
		treeWidget->clear();
		bonusClipTreeWidget->clear();

		checkForValidFields();

		const QFileInfo fileInfo(fileName);
		const bool isCompressedPackage = fileInfo.suffix().toLower() == "pkgc";
		const bool isDescriptionFile = fileInfo.suffix().toLower() == "xml";

		if (isCompressedPackage)
		{
			TmpDir tmpDir(new QTemporaryDir());
			qDebug() << "Loading compressed clips package with clips dir:" << tmpDir->path();

			if (!this->m_clipPackage->loadClipsFromCompressedArchive(fileName, tmpDir->path()))
			{
				QMessageBox::critical(this, tr("Error"), tr("Error on loading compressed package."));

				return;
			}

			this->m_extractDir.swap(tmpDir);
		}
		else if (isDescriptionFile)
		{
			if (!this->m_clipPackage->loadClipsFromFile(fileName))
			{
				QMessageBox::critical(this, tr("Error"), tr("Error on loading clip description file."));

				return;
			}
		}

		loadClipPackage();
	}
}

void ClipPackageEditor::saveAs()
{
	const QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), this->m_dir, tr("All Files (*);;Compressed Clip Package (*.pkgc);;Clip Package File (*.xml)"));

	if (!fileName.isEmpty())
	{
		const QFileInfo fileInfo(fileName);

		if (fileInfo.suffix().toLower() == "pkgc")
		{
			if (!this->m_clipPackage->saveClipsToCompressedArchive(fileName))
			{
				QMessageBox::critical(this, tr("Error"), tr("Error on saving compressed package."));
			}
		}
		else if (!this->m_clipPackage->saveClipsToFile(fileName))
		{
			QMessageBox::critical(this, tr("Error"), tr("Error on saving clip package."));
		}
	}
}

void ClipPackageEditor::save()
{
	const QString fileName = this->m_clipPackage->filePath();

	if (!fileName.isEmpty())
	{
		const QFileInfo fileInfo(fileName);

		if (fileInfo.suffix().toLower() == "pkgc")
		{
			if (!this->m_clipPackage->saveClipsToCompressedArchive(fileName))
			{
				QMessageBox::critical(this, tr("Error"), tr("Error on saving compressed package."));
			}
		}
		else if (!this->m_clipPackage->saveClipsToFile(fileName))
		{
			QMessageBox::critical(this, tr("Error"), tr("Error on saving clip package."));
		}
	}
}

void ClipPackageEditor::closePackage()
{
	if (QMessageBox::question(this, tr("Closing Package"), tr("Are you sure?")) == QMessageBox::Yes)
	{
		this->idLineEdit->clear();
		this->m_clipPackage->clear();
		this->treeWidget->clear();

		checkForValidFields();
	}
}

void ClipPackageEditor::changedCurrentClipItem(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	this->bonusClipTreeWidget->selectionModel()->clearSelection();
	const bool hasSelectedClip = (current != nullptr);
	editClipPushButton->setEnabled(hasSelectedClip);
	removeClipPushButton->setEnabled(hasSelectedClip);
}

void ClipPackageEditor::changedCurrentBonusClipItem(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	this->treeWidget->selectionModel()->clearSelection();
	const bool hasSelectedClip = (current != nullptr);
	editClipPushButton->setEnabled(hasSelectedClip);
	removeClipPushButton->setEnabled(hasSelectedClip);
}

ClipPackageEditor::ClipPackageEditor(fairytale *app, QWidget* parent) : QDialog(parent), m_app(app), m_clipEditor(nullptr), m_clipPackage(new ClipPackage(app, this))
{
	setupUi(this);

	connect(this->idLineEdit, &QLineEdit::textChanged, this, &ClipPackageEditor::idChanged);
	connect(this->loadCustomClipsPackagePushButton, &QPushButton::clicked, this, &ClipPackageEditor::loadCustomClipsPackage);
	connect(this->addClipPushButton, &QPushButton::clicked, this, &ClipPackageEditor::addClip);
	connect(this->editClipPushButton, &QPushButton::clicked, this, &ClipPackageEditor::editClip);
	connect(this->removeClipPushButton, &QPushButton::clicked, this, &ClipPackageEditor::removeClip);
	connect(this->newPackagePushButton, &QPushButton::clicked, this, &ClipPackageEditor::newPackage);
	connect(this->loadPackagePushButton, &QPushButton::clicked, this, &ClipPackageEditor::loadPackage);
	connect(this->saveAsPushButton, &QPushButton::clicked, this, &ClipPackageEditor::saveAs);
	connect(this->savePushButton, &QPushButton::clicked, this, &ClipPackageEditor::save);
	connect(this->closePackagePushButton, &QPushButton::clicked, this, &ClipPackageEditor::closePackage);

	connect(this->okPushButton, &QPushButton::clicked, this, &ClipPackageEditor::accept);
	connect(this->cancelPushButton, &QPushButton::clicked, this, &ClipPackageEditor::reject);

	connect(this->treeWidget, &QTreeWidget::currentItemChanged, this, &ClipPackageEditor::changedCurrentClipItem);
	connect(this->bonusClipTreeWidget, &QTreeWidget::currentItemChanged, this, &ClipPackageEditor::changedCurrentBonusClipItem);

	QSettings settings("fairytale");
	m_dir = settings.value("clippackageeditordir").toString();

	checkForValidFields();
}

ClipPackageEditor::~ClipPackageEditor()
{
	QSettings settings("fairytale");
	settings.setValue("clippackageeditordir", m_dir);
}

ClipEditor* ClipPackageEditor::clipEditor()
{
	if (this->m_clipEditor == nullptr)
	{
		this->m_clipEditor = new ClipEditor(m_app, this);
	}

	return this->m_clipEditor;
}

bool ClipPackageEditor::checkForValidFields()
{
	bool result = !this->m_clipPackage->id().isEmpty();
	this->saveAsPushButton->setEnabled(result);
	this->savePushButton->setEnabled(result && !this->m_clipPackage->filePath().isEmpty());

	return result;
}

void ClipPackageEditor::loadClipPackage()
{
	this->idLineEdit->setText(this->m_clipPackage->id());

	for (ClipPackage::Clips::const_iterator iterator = this->m_clipPackage->clips().begin(); iterator != this->m_clipPackage->clips().end(); ++iterator)
	{
		const Clip *clip = iterator.value();

		QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
		const QUrl imageUrl = this->m_app->resolveClipUrl(clip->imageUrl());
		item->setIcon(0, QIcon(fairytale::filePath(imageUrl)));
		item->setText(0, clip->description());
		item->setData(0, Qt::UserRole, clip->id());
		treeWidget->addTopLevelItem(item);
	}

	for (ClipPackage::BonusClips::const_iterator iterator = this->m_clipPackage->bonusClips().begin(); iterator != this->m_clipPackage->bonusClips().end(); ++iterator)
	{
		const BonusClip *clip = iterator.value();

		QTreeWidgetItem *item = new QTreeWidgetItem(bonusClipTreeWidget);
		const QUrl imageUrl = this->m_app->resolveClipUrl(clip->imageUrl());
		item->setIcon(0, QIcon(fairytale::filePath(imageUrl)));
		item->setText(0, clip->description());
		item->setData(0, Qt::UserRole, clip->id());
		bonusClipTreeWidget->addTopLevelItem(item);
	}

	checkForValidFields();
}