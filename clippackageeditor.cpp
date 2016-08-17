#include <iostream>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "clippackageeditor.h"
#include "clipeditor.h"
#include "clippackage.h"
#include "clip.h"

#include "clippackageeditor.moc"
#include "fairytale.h"

void ClipPackageEditor::addClip()
{

	if (this->clipEditor()->exec() == QDialog::Accepted)
	{
		Clip *clip = this->clipEditor()->clip(m_clipPackage);
		m_clipPackage->addClip(clip);
		QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
		item->setText(0, clip->description());
		treeWidget->addTopLevelItem(item);

		std::cerr << "Adding clip to tree widget with description " << clip->description().toUtf8().constData() << std::endl;
	}
}

void ClipPackageEditor::editClip()
{
	if (!treeWidget->selectedItems().isEmpty())
	{
		QTreeWidgetItem *item = treeWidget->selectedItems().first();
		const int index = treeWidget->indexOfTopLevelItem(item);
		Clip *clip = this->m_clipPackage->clips().at(index);

		this->clipEditor()->fill(clip);

		if (this->clipEditor()->exec() == QDialog::Accepted)
		{
			this->clipEditor()->assignToClip(clip);
		}
	}
}

void ClipPackageEditor::removeClip()
{
	if (!treeWidget->selectedItems().isEmpty())
	{
		QTreeWidgetItem *item = treeWidget->selectedItems().first();
		const int index = treeWidget->indexOfTopLevelItem(item);
		Clip *clip = this->m_clipPackage->clips().at(index);
		this->m_clipPackage->clips().removeAt(index);

		delete clip;
		clip = nullptr;

		delete item;
		item = nullptr;
	}
}

void ClipPackageEditor::loadPackage()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"), QString(), tr("All Files (*);;Clip Description (*.xml);;Compressed Clip Package (*.pkgc)"));

	if (!fileName.isEmpty())
	{
		this->m_clipPackage->clear();
		treeWidget->clear();

		const QFileInfo fileInfo(fileName);
		const bool isCompressedPackage = fileInfo.suffix().toLower() == "pkgc";
		const bool isDescriptionFile = fileInfo.suffix().toLower() == "xml";

		if (isCompressedPackage)
		{
			if (!this->m_clipPackage->loadClipsFromCompressedArchive(fileName, this->app()->clipsDir().toLocalFile()))
			{
				QMessageBox::critical(this, tr("Error"), tr("Error on loading compressed package."));

				return;
			}
		}
		else if (isDescriptionFile)
		{
			if (!this->m_clipPackage->loadClipsFromFile(fileName))
			{
				QMessageBox::critical(this, tr("Error"), tr("Error on loading clip description file."));

				return;
			}
		}

		for (int i = 0; i < this->m_clipPackage->clips().size(); ++i)
		{
			QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
			item->setText(0, this->m_clipPackage->clips().at(i)->description());
			treeWidget->addTopLevelItem(item);
		}
	}
}

void ClipPackageEditor::saveAs()
{
	const QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), QString(), tr("All Files (*);;Compressed Clip Package (*.pkgc)"));

	if (!fileName.isEmpty())
	{
		if (!this->m_clipPackage->saveClipsToCompressedArchive(fileName))
		{
			QMessageBox::critical(this, tr("Error"), tr("Error on saving compressed package."));
		}
	}
}

void ClipPackageEditor::closePackage()
{
	if (QMessageBox::question(this, tr("Closing Package"), tr("Are you sure?")) == QMessageBox::Yes)
	{
		this->m_clipPackage->clear();
		this->treeWidget->clear();
	}
}

void ClipPackageEditor::changedCurrentItem(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	const bool hasSelectedClip = (current != nullptr);
	editClipPushButton->setEnabled(hasSelectedClip);
	removeClipPushButton->setEnabled(hasSelectedClip);
}

ClipPackageEditor::ClipPackageEditor(fairytale *app, QWidget* parent) : QDialog(parent), m_app(app), m_clipEditor(nullptr), m_clipPackage(new ClipPackage(app, this))
{
	setupUi(this);

	connect(this->addClipPushButton, SIGNAL(clicked()), this, SLOT(addClip()));
	connect(this->editClipPushButton, SIGNAL(clicked()), this, SLOT(editClip()));
	connect(this->removeClipPushButton, SIGNAL(clicked()), this, SLOT(removeClip()));
	connect(this->loadPackagePushButton, SIGNAL(clicked()), this, SLOT(loadPackage()));
	connect(this->saveAsPushButton, SIGNAL(clicked()), this, SLOT(saveAs()));
	connect(this->closePackagePushButton, SIGNAL(clicked()), this, SLOT(closePackage()));

	connect(this->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(this->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(this->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(changedCurrentItem(QTreeWidgetItem*,QTreeWidgetItem*)));
}

ClipEditor* ClipPackageEditor::clipEditor()
{
	if (this->m_clipEditor == nullptr)
	{
		this->m_clipEditor = new ClipEditor(m_app, this);
	}

	return this->m_clipEditor;
}
