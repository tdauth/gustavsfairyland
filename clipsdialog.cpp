#include <QtWidgets/QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

#include "clipsdialog.h"
#include "clippackage.h"

#include "clipsdialog.moc"
#include "fairytale.h"
#include "clip.h"
#include "bonusclip.h"

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
					packages.insert(clipPackage->id(), clipPackage);
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

void ClipsDialog::removeSelected()
{
    foreach (QTreeWidgetItem *item, this->treeWidget->selectedItems())
    {
        ClipPackages::iterator iterator = this->m_clipPackages.find(item);

        if (iterator != this->m_clipPackages.end())
        {
            ClipPackage *package = iterator.value();

            qDebug() << "Before removing package";
            this->m_app->removeClipPackage(package);

            qDebug() << "Before erasing item";
            // erase package entry
            this->m_clipPackages.erase(iterator);

            // drop all children from the map
            for (int i = 0; i < item->childCount(); ++i)
            {
                this->m_clips.remove(item->child(i));
            }

            // delete top level widget
            delete item;
            // Remove clip package from memory.
            delete package;
        }
    }
}

void ClipsDialog::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	Clips::iterator iterator = m_clips.find(item);

	if (iterator != m_clips.end())
	{
		const QUrl url = this->m_app->resolveClipUrl(iterator.value()->imageUrl());
		qDebug() << "Open URL: " << url;
		// TODO just popup a modal dialog with the image
		QDesktopServices::openUrl(url);

	}
}

ClipsDialog::ClipsDialog(fairytale *app, QWidget* parent) : QDialog(parent), m_app(app)
{
	setupUi(this);
	connect(this->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(this->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(this->addFilePushButton, SIGNAL(clicked()), this, SLOT(addFile()));
	connect(this->addDirectoryPushButton, SIGNAL(clicked()), this, SLOT(addDirectory()));
	connect(this->removePushButton, &QPushButton::clicked, this, &ClipsDialog::removeSelected);

	connect(this->treeWidget, &QTreeWidget::itemDoubleClicked, this, &ClipsDialog::itemDoubleClicked);
}

void ClipsDialog::fill(const fairytale::ClipPackages &packages)
{
	this->treeWidget->clear();
	this->m_clipPackages.clear();
	this->m_clips.clear();
	this->m_bonusClips.clear();

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
	m_clipPackages.insert(topLevelItem, package);

	QTreeWidgetItem *personsItem = new QTreeWidgetItem(topLevelItem);
	personsItem->setText(0, tr("Persons"));
	QTreeWidgetItem *actsItem = new QTreeWidgetItem(topLevelItem);
	actsItem->setText(0, tr("Acts"));
	QTreeWidgetItem *bonusesItem = new QTreeWidgetItem(topLevelItem);
	bonusesItem->setText(0, tr("Bonuses"));

	int persons = 0;
	int acts = 0;
	int bonuses = 0;

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

	foreach (BonusClip *clip, package->bonusClips())
	{
		QTreeWidgetItem *clipItem = new QTreeWidgetItem(bonusesItem);
		this->m_bonusClips.insert(clipItem, clip);
		clipItem->setText(0, clip->description());
		clipItem->setIcon(0, QIcon(m_app->resolveClipUrl(clip->imageUrl()).toLocalFile()));

		++bonuses;
	}

	topLevelItem->setText(1, QString::number(persons + acts));
	topLevelItem->setText(2, QString::number(package->rounds()));
	personsItem->setText(1, QString::number(persons));
	actsItem->setText(1, QString::number(acts));
	bonusesItem->setText(1, QString::number(bonuses));
}
