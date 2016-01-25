#include <QtWidgets/QFileDialog>
#include <QMessageBox>

#include "clipsdialog.h"
#include "clippackage.h"

#include "clipsdialog.moc"
#include "fairytale.h"
#include "clip.h"

void ClipsDialog::addFile()
{
	const QString filePath = QFileDialog::getOpenFileName(this, tr("Add Clip"), "", tr("Clip Files (*.xml)"));

	if (!filePath.isEmpty())
	{
		ClipPackage *clipPackage = new ClipPackage(this->m_app);

		if (clipPackage->loadClipsFromFile(filePath))
		{
			this->m_app->addClipPackage(clipPackage);
			this->fill(clipPackage);
		}
		else
		{
			delete clipPackage;
			clipPackage = nullptr;
		}
	}
}

void ClipsDialog::addDirectory()
{
	const QString dirPath = QFileDialog::getExistingDirectory(this, tr("Clips Directory"), this->m_app->clipsDir().toLocalFile());

	if (!dirPath.isEmpty())
	{
		QDir dir(dirPath);
		QStringList filters;
		filters << "*.xml";
		dir.setNameFilters(filters);

		fairytale::ClipPackages packages;

		foreach (QFileInfo fileInfo, dir.entryInfoList())
		{
			if (fileInfo.isReadable())
			{
				ClipPackage *clipPackage = new ClipPackage(this->m_app);

				if (clipPackage->loadClipsFromFile(fileInfo.absoluteFilePath()))
				{
					packages.push_back(clipPackage);
				}
				else
				{
					delete clipPackage;
					clipPackage = nullptr;
				}
			}
		}

		if (!packages.isEmpty())
		{
			this->m_app->setClipPackages(packages);
			this->fill(packages);
		}
		else
		{
			QMessageBox::warning(this, tr("Missing Clips"), tr("Missing clips in the directory."));
		}
	}
}

ClipsDialog::ClipsDialog(fairytale *app, QWidget* parent) : QDialog(parent), m_app(app)
{
	setupUi(this);
	connect(this->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(this->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(this->addFilePushButton, SIGNAL(clicked()), this, SLOT(addFile()));
	connect(this->addDirectoryPushButton, SIGNAL(clicked()), this, SLOT(addDirectory()));
}

void ClipsDialog::fill(const fairytale::ClipPackages& packages)
{
	this->treeWidget->clear();

	foreach (ClipPackage *package, packages)
	{
		this->fill(package);
	}
}

void ClipsDialog::fill(ClipPackage* package)
{
	QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(this->treeWidget);
	topLevelItem->setText(0, package->name());
	topLevelItem->setText(1, QString::number(package->clips().size()));
	this->treeWidget->addTopLevelItem(topLevelItem);

	foreach (Clip *clip, package->clips())
	{
		QTreeWidgetItem *clipItem = new QTreeWidgetItem(topLevelItem);
		clipItem->setText(0, clip->description());
		clipItem->setIcon(0, QIcon(m_app->resolveClipUrl(clip->imageUrl()).toLocalFile()));
	}
}
