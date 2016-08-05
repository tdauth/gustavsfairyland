#include <QtWidgets/QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

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

void ClipsDialog::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	Clips::iterator iterator = m_clips.find(item);

	if (iterator != m_clips.end())
	{
		// TODO just popup a modal dialog with the image
		QDesktopServices::openUrl((*iterator)->imageUrl());
	}
}

ClipsDialog::ClipsDialog(fairytale *app, QWidget* parent) : QDialog(parent), m_app(app)
{
	setupUi(this);
	connect(this->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(this->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(this->addFilePushButton, SIGNAL(clicked()), this, SLOT(addFile()));
	connect(this->addDirectoryPushButton, SIGNAL(clicked()), this, SLOT(addDirectory()));

	connect(this->treeWidget, &QTreeWidget::itemDoubleClicked, this, &ClipsDialog::itemDoubleClicked);
}

void ClipsDialog::fill(const fairytale::ClipPackages &packages)
{
	this->treeWidget->clear();
	this->m_clips.clear();

	foreach (ClipPackage *package, packages)
	{
		this->fill(package);
	}
}

void ClipsDialog::fill(ClipPackage* package)
{
	QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(this->treeWidget);
	topLevelItem->setText(0, package->name());
	this->treeWidget->addTopLevelItem(topLevelItem);

	QTreeWidgetItem *personsItem = new QTreeWidgetItem(topLevelItem);
	personsItem->setText(0, tr("Persons"));
	//topLevelItem->setText(1, QString::number(package->clips().size()));
	QTreeWidgetItem *actsItem = new QTreeWidgetItem(topLevelItem);
	actsItem->setText(0, tr("Acts"));
	//topLevelItem->setText(1, QString::number(package->clips().size()));

	int persons = 0;
	int acts = 0;

	foreach (Clip *clip, package->clips())
	{
		QTreeWidgetItem *clipItem = new QTreeWidgetItem(clip->isPerson() ? personsItem : actsItem);
		this->m_clips.insert(clipItem, clip);
		clipItem->setText(0, clip->description());
		clipItem->setIcon(0, QIcon(m_app->resolveClipUrl(clip->imageUrl()).toLocalFile()));

		if (clip->isPerson())
		{
			++persons;
		}
		else
		{
			++acts;
		}
	}

	topLevelItem->setText(1, QString::number(persons + acts));
	personsItem->setText(1, QString::number(persons));
	actsItem->setText(1, QString::number(acts));
}
