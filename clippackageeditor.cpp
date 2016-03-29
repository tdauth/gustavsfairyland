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
	if (this->m_clipEditor == nullptr)
	{
		this->m_clipEditor = new ClipEditor(this);
	}

	if (this->m_clipEditor->exec() == QDialog::Accepted)
	{
		Clip *clip = this->m_clipEditor->clip(m_clipPackage);
		m_clipPackage->addClip(clip);
		QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
		item->setText(0, clip->description());
		treeWidget->addTopLevelItem(item);

		std::cerr << "Adding clip to tree widget with description " << clip->description().toUtf8().constData() << std::endl;
	}
}

void ClipPackageEditor::loadPackage()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Open Package"), QString(), tr("All Files (*);;Compressed Clip Package (*.pkgc)"));

	if (!fileName.isEmpty())
	{
		if (!this->m_clipPackage->loadClipsFromCompressedArchive(fileName, this->app()->clipsDir().toLocalFile()))
		{
			QMessageBox::critical(this, tr("Error"), tr("Error on loading compressed package."));
		}
		else
		{
			treeWidget->clear();

			for (int i = 0; i < this->m_clipPackage->clips().size(); ++i)
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
				item->setText(0, this->m_clipPackage->clips().at(i)->description());
				treeWidget->addTopLevelItem(item);
			}
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

ClipPackageEditor::ClipPackageEditor(fairytale *app, QWidget* parent) : QDialog(parent), m_app(app), m_clipEditor(nullptr), m_clipPackage(new ClipPackage(this))
{
	setupUi(this);

	connect(this->addClipPushButton, SIGNAL(clicked()), this, SLOT(addClip()));
	connect(this->loadPackagePushButton, SIGNAL(clicked()), this, SLOT(loadPackage()));
	connect(this->saveAsPushButton, SIGNAL(clicked()), this, SLOT(saveAs()));

	connect(this->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(this->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}