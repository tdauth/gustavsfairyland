#include <iostream>

#include <QtWidgets>
#include <QDir>

#include "settingsdialog.h"
#include "clippackage.h"
#include "clip.h"
#include "bonusclip.h"
#include "customfairytale.h"
#include "gamemode.h"
#include "gamemodemoving.h"

void SettingsDialog::restoreDefaults()
{
	musicCheckBox->setChecked(true);
	clickSoundsCheckBox->setChecked(true);

#ifndef Q_OS_ANDROID
	fullScreenCheckBox->setChecked(true);
#else
	fullScreenCheckBox->setChecked(false);
#endif

	m_clipsDir = QUrl::fromLocalFile(m_app->defaultClipsDirectory());
	this->clipsDirectoryLabel->setText(m_clipsDir.toString());

	if (this->m_app->loadDefaultClipPackage())
	{
		// Remove all clip packages.
		for (ClipPackages::iterator iterator = this->m_clipPackages.begin(); iterator != this->m_clipPackages.end(); )
		{
			QTreeWidgetItem *item = iterator.key();
			ClipPackage *package = iterator.value();

			qDebug() << "Before removing package";
			this->m_app->removeClipPackage(package);

			qDebug() << "Before erasing item";
			// erase package entry
			iterator = this->m_clipPackages.erase(iterator);

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

		Q_ASSERT(!m_app->clipPackages().isEmpty());
		this->fill(m_app->clipPackages());
	}
}

void SettingsDialog::changeClipsDirectory()
{
	const QUrl clipsDirUrl = QFileDialog::getExistingDirectoryUrl(this, tr("Clips Directory"));

	if (!clipsDirUrl.isEmpty() && clipsDirUrl.isValid())
	{
		m_clipsDir = clipsDirUrl;
		this->clipsDirectoryLabel->setText(m_clipsDir.toString());
	}
}

void SettingsDialog::apply()
{
	this->m_app->setMusicMuted(!musicCheckBox->isChecked());

	fairytale::GameModes::const_iterator iterator = this->m_app->gameModes().find("pagesontheground");

	if (iterator != this->m_app->gameModes().end())
	{
		GameModeMoving *gameModeMoving = dynamic_cast<GameModeMoving*>(iterator.value());

		if (gameModeMoving != nullptr)
		{
			gameModeMoving->setPlayClickSounds(clickSoundsCheckBox->isChecked());
		}
		else
		{
			QMessageBox::warning(this, tr("Game mode is missing"), tr("Game mode \"Moving Pages on the Ground\" is missing. One option has no effect."));
		}
	}
	else
	{
		QMessageBox::warning(this, tr("Game mode is missing"), tr("Game mode \"Moving Pages on the Ground\" is missing. One option has no effect."));
	}

	if (this->fullScreenCheckBox->isChecked())
	{
		this->m_app->showFullScreen();
	}
	else
	{
		this->m_app->showNormal();
	}

	this->m_app->setClipsDir(m_clipsDir);

	// Set all specified clip packages for the application.
	fairytale::ClipPackages clipPackages;

	foreach (ClipPackage *clipPackage, m_clipPackages.values())
	{
		clipPackages.insert(clipPackage->id(), clipPackage);
	}

	this->m_app->setClipPackages(clipPackages);

	// Get all custom fairytales which are still in use.
	QSet<QString> customFairytaleNames;

	for (int i = 0; i < this->customFairytalesListWidget->count(); ++i)
	{
		customFairytaleNames << this->customFairytalesListWidget->item(i)->data(Qt::UserRole).toString();
	}

	// Remove all custom fairy tales which are not in the list anymore
	QSet<QString> customFairytaleNamesToRemove;

	// dont remove already, since the iterator becomes invalid after removal
	for (fairytale::CustomFairytales::const_iterator iterator = m_app->customFairytales().constBegin(); iterator != m_app->customFairytales().constEnd(); ++iterator)
	{
		if (!customFairytaleNames.contains(iterator.key()))
		{
			customFairytaleNamesToRemove << iterator.key();
		}
	}

	foreach (QString id, customFairytaleNamesToRemove)
	{
		fairytale::CustomFairytales::const_iterator iterator = m_app->customFairytales().find(id);

		if (iterator != m_app->customFairytales().constEnd())
		{
			m_app->removeCustomFairytale(iterator.value());
		}
	}
}

void SettingsDialog::update()
{
	this->musicCheckBox->setChecked(!this->m_app->isMusicMuted());

	fairytale::GameModes::const_iterator iterator = this->m_app->gameModes().find("pagesontheground");

	if (iterator != this->m_app->gameModes().end())
	{
		GameModeMoving *gameModeMoving = dynamic_cast<GameModeMoving*>(iterator.value());

		if (gameModeMoving != nullptr)
		{
			clickSoundsCheckBox->setChecked(gameModeMoving->playClickSounds());
		}
		else
		{
			QMessageBox::warning(this, tr("Game mode is missing"), tr("Game mode \"Moving Pages on the Ground\" is missing. One option has no effect."));
		}
	}
	else
	{
		QMessageBox::warning(this, tr("Game mode is missing"), tr("Game mode \"Moving Pages on the Ground\" is missing. One option has no effect."));
	}

	this->fullScreenCheckBox->setChecked(this->m_app->isFullScreen());
	this->m_clipsDir = this->m_app->clipsDir();
	this->clipsDirectoryLabel->setText(m_clipsDir.toString());

	this->fill(m_app->clipPackages());

	customFairytalesListWidget->clear();

	for (fairytale::CustomFairytales::const_iterator iterator = m_app->customFairytales().constBegin(); iterator != m_app->customFairytales().constEnd(); ++iterator)
	{
		QListWidgetItem *item = new QListWidgetItem(iterator.value()->name());
		item->setData(Qt::UserRole, iterator.value()->name());
		customFairytalesListWidget->addItem(item);
	}
}

void SettingsDialog::addFile()
{
	const QString filePath = QFileDialog::getOpenFileName(this, tr("Add Clip"), "", tr("Clip Files (*.xml)"));

	if (!filePath.isEmpty())
	{
		ClipPackage *clipPackage = new ClipPackage(this->m_app);

		if (clipPackage->loadClipsFromFile(filePath))
		{
			this->fill(clipPackage);
		}
		else
		{
			delete clipPackage;
			clipPackage = nullptr;
		}
	}
}

void SettingsDialog::addDirectory()
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
			this->fill(packages);
		}
		else
		{
			QMessageBox::warning(this, tr("Missing Clips"), tr("Missing clips in the directory."));
		}
	}
}

void SettingsDialog::removeSelected()
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

void SettingsDialog::copyCustomFairytale()
{
	if (!this->customFairytalesListWidget->selectedItems().isEmpty())
	{
		QListWidgetItem *item = this->customFairytalesListWidget->selectedItems().front();

		fairytale::CustomFairytales::const_iterator iterator = m_app->customFairytales().find(item->data(Qt::UserRole).toString());

		if (iterator != m_app->customFairytales().end())
		{
			CustomFairytale *customFairytale = iterator.value();
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(customFairytale->toString());
		}
	}
}

void SettingsDialog::importCustomFairytale()
{
	QClipboard *clipboard = QApplication::clipboard();
	const QString importText = clipboard->text();

	std::cerr << "Importing: " << importText.toStdString() << std::endl;

	if (!importText.isEmpty())
	{
		importCustomFairytaleEx(importText);
	}
}

void SettingsDialog::importCustomFairytaleEx(const QString &importText)
{
	bool ok = false;
	const QString text = QInputDialog::getText(this, tr("Custom Fairytale Name"), tr("Name of custom fairytale:"), QLineEdit::Normal, "", &ok);

	if (ok && !text.isEmpty())
	{
		// Check if name is already used. It must be unique since it is used as key.
		if (m_app->customFairytales().find(text) != m_app->customFairytales().end())
		{
			if (QMessageBox::question(this, tr("Overwrite existing custom fairytale?"), tr("Do you want to overwrite the existing custom fairytale?")) == QMessageBox::No)
			{
				// Select another name. TODO clipboard might have changed?!
				importCustomFairytaleEx(importText);

				return;
			}
		}

		CustomFairytale *customFairytale = CustomFairytale::fromString(importText, text, m_app);

		if (customFairytale != nullptr)
		{
			std::cerr << "Is not null, adding it!" << std::endl;

			m_app->addCustomFairytale(customFairytale);

			fairytale::CustomFairytales::const_iterator iterator = m_app->customFairytales().find(text);

			if (iterator != m_app->customFairytales().constEnd())
			{
				QListWidgetItem *item = new QListWidgetItem(iterator.value()->name());
				item->setData(Qt::UserRole, iterator.value()->name());
				customFairytalesListWidget->addItem(item);
			}
		}
	}
}

void SettingsDialog::removeCustomFairytale()
{
	foreach (QListWidgetItem *item, this->customFairytalesListWidget->selectedItems())
	{
		delete item;
	}
}

void SettingsDialog::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	Clips::iterator iterator = m_clips.find(item);

	if (iterator != m_clips.end())
	{
		const QUrl url = this->m_app->resolveClipUrl(iterator.value()->imageUrl());
		qDebug() << "Open URL: " << url;
		// TODO just popup a modal dialog with the image
		QDesktopServices::openUrl(url);
	}
	else
	{
		BonusClips::iterator bonusClipsIterator = m_bonusClips.find(item);

		if (bonusClipsIterator != m_bonusClips.end())
		{
			const QUrl url = this->m_app->resolveClipUrl(bonusClipsIterator.value()->imageUrl());
			qDebug() << "Open URL: " << url;
			// TODO just popup a modal dialog with the image
			QDesktopServices::openUrl(url);
		}
	}
}

SettingsDialog::SettingsDialog(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app)
{
	setupUi(this);

	connect(this->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &SettingsDialog::restoreDefaults);
	connect(this->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsDialog::accept);
	connect(this->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsDialog::apply);
	connect(this->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
	connect(this->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SettingsDialog::reject);

	connect(this->clipsDirectoryPushButton, &QPushButton::clicked, this, &SettingsDialog::changeClipsDirectory);

	connect(this->addFilePushButton, SIGNAL(clicked()), this, SLOT(addFile()));
	connect(this->addDirectoryPushButton, SIGNAL(clicked()), this, SLOT(addDirectory()));
	connect(this->removePushButton, &QPushButton::clicked, this, &SettingsDialog::removeSelected);
	connect(this->treeWidget, &QTreeWidget::itemDoubleClicked, this, &SettingsDialog::itemDoubleClicked);

	connect(this->copyCustomFairytalePushButton, &QPushButton::clicked, this, &SettingsDialog::copyCustomFairytale);
	connect(this->importCustomFairytalePushButton, &QPushButton::clicked, this, &SettingsDialog::importCustomFairytale);
	connect(this->removeCustomFairytalePushButton, &QPushButton::clicked, this, &SettingsDialog::removeCustomFairytale);
}

void SettingsDialog::fill(const fairytale::ClipPackages &packages)
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

void SettingsDialog::fill(ClipPackage *package)
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

void SettingsDialog::load(QSettings &settings)
{
	settings.beginGroup("settings");

	const QDir defaultClipsDir(m_app->defaultClipsDirectory());
	qDebug() << "Default clips dir:" << m_app->defaultClipsDirectory();
	// the default path is the "clips" sub directory
#ifndef Q_OS_ANDROID
	m_clipsDir = QUrl::fromLocalFile(settings.value("clipsDir", defaultClipsDir.absolutePath()).toString());
#else
	m_clipsDir = QUrl(settings.value("clipsDir", defaultClipsDir.absolutePath()).toString());
#endif
	this->musicCheckBox->setChecked(settings.value("music", true).toBool());
	this->clickSoundsCheckBox->setChecked(settings.value("clickSounds", true).toBool());
// showing maximized or fullscren leads to menu actions disappearing on a smartphone
#ifndef Q_OS_ANDROID
	const bool defaultShowFullScreen = false;
#else
	const bool defaultShowFullScreen = true;
#endif
	this->fullScreenCheckBox->setChecked(settings.value("fullscreen", defaultShowFullScreen).toBool());

	const int size = settings.beginReadArray("clipPackages");

	if (size > 0)
	{
		for (int i = 0; i < size; ++i)
		{
			settings.setArrayIndex(i);
			const QString filePath = settings.value("filePath").toString();

			ClipPackage *package = new ClipPackage(m_app);

			if (package->loadClipsFromFile(filePath))
			{
				this->fill(package);
			}
			else
			{
				delete package;
				package = nullptr;
			}
		}
	}

	settings.endArray();

	// default package
	if (m_app->clipPackages().isEmpty())
	{
		m_app->loadDefaultClipPackage();
	}

	// Apply before loading custom fairytales since they are not stored in the settings additionally and otherwise they would all be deleted.
	apply();

	const int customFairytalesSize = settings.beginReadArray("customfairytales");

	for (int i = 0; i < customFairytalesSize; ++i)
	{
		settings.setArrayIndex(i);
		CustomFairytale *customFairytale = new CustomFairytale(m_app);
		customFairytale->load(settings);
		m_app->addCustomFairytale(customFairytale);
	}

	settings.endArray();

	settings.endGroup();
}

void SettingsDialog::save(QSettings &settings)
{
	settings.beginGroup("settings");
#ifndef Q_OS_ANDROID
	settings.setValue("clipsDir", m_clipsDir.toLocalFile());
#else
	settings.setValue("clipsDir", m_clipsDir.toString());
#endif
	settings.setValue("music", this->musicCheckBox->isChecked());
	settings.setValue("clickSounds", this->clickSoundsCheckBox->isChecked());
	settings.setValue("fullscreen", this->fullScreenCheckBox->isChecked());

	settings.beginWriteArray("clipPackages", this->m_clipPackages.size());
	int i = 0;

	for (ClipPackages::const_iterator iterator = this->m_clipPackages.begin(); iterator != this->m_clipPackages.end(); ++iterator)
	{
		settings.setArrayIndex(i);
		settings.setValue("filePath", (*iterator)->filePath());

		++i;
	}

	settings.endArray();

	settings.beginWriteArray("customfairytales");
	i= 0;

	for (fairytale::CustomFairytales::const_iterator iterator = m_app->customFairytales().constBegin(); iterator != m_app->customFairytales().constEnd(); ++iterator)
	{
		settings.setArrayIndex(i);
		iterator.value()->save(settings);

		++i;
	}

	settings.endArray();

	settings.endGroup();
}

void SettingsDialog::changeEvent(QEvent* event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			std::cerr << "Retranslate UI of about dialog" << std::endl;
			this->retranslateUi(this);

			break;
		}

		default:
		{
			break;
		}
	}

	QDialog::changeEvent(event);
}
