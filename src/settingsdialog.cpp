#include <QtCore>
#include <QtWidgets>
#include <QtMultimedia>

#include "settingsdialog.h"
#include "clippackage.h"
#include "clip.h"
#include "bonusclip.h"
#include "customfairytale.h"
#include "gamemode.h"
#include "gamemodemoving.h"
#include "highscores.h"

void SettingsDialog::restoreDefaults()
{
#ifndef Q_OS_ANDROID
	fullScreenCheckBox->setChecked(true);
	// We need file:/ on other systems.
	m_clipsDir = QUrl::fromLocalFile(this->app()->defaultClipsDirectory());
#else
	fullScreenCheckBox->setChecked(false);
	// Dont prepend file:/ on Android!
	m_clipsDir = QUrl(this->app()->defaultClipsDirectory());
#endif

	// Sound
	musicCheckBox->setChecked(true);
	musicVolumeSpinBox->setValue(fairytale::defaultMusicVolume);
	clickSoundsCheckBox->setChecked(true);
	clickSoundsVolumeSpinBox->setValue(fairytale::defaultClickSoundsVolume);
	videoSoundCheckBox->setChecked(true);
	videoSoundVolumeSpinBox->setValue(fairytale::defaultVideoSoundVolume);

	// Clips
	this->clipsDirectoryLabel->setText(m_clipsDir.toString());

	// TODO unload only on apply!
	// Remove all clip packages.
	for (ClipPackages::iterator iterator = this->m_clipPackages.begin(); iterator != this->m_clipPackages.end(); )
	{
		QTreeWidgetItem *item = iterator.key();
		ClipPackage *package = iterator.value();

		qDebug() << "Before removing package";
		this->app()->removeClipPackage(package);

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

	if (this->app()->loadDefaultClipPackage())
	{
		Q_ASSERT(!this->app()->clipPackages().isEmpty());
		this->fill(this->app()->clipPackages());
	}
}

void SettingsDialog::changeClipsDirectory()
{
	const QUrl clipsDirUrl = QFileDialog::getExistingDirectoryUrl(this, tr("Clips Directory"), m_clipsDir);

	if (!clipsDirUrl.isEmpty() && clipsDirUrl.isValid())
	{
		m_clipsDir = clipsDirUrl;
		this->clipsDirectoryLabel->setText(m_clipsDir.toString());
	}
}

void SettingsDialog::apply()
{
	if (this->fullScreenCheckBox->isChecked())
	{
		// TODO Workaround: in Fullscreen mode on Windows 7 repaint() does not cause immediate paintEvent() call! Works only when showing it and then calling showFullScreen().
		this->app()->show();
		this->app()->showFullScreen();
	}
	else
	{
		this->app()->showNormal();
	}

	// Sound
	this->app()->setMusicMuted(!musicCheckBox->isChecked());
	this->app()->setMusicVolume(musicVolumeSpinBox->value());
	this->app()->setAudioPlayerMuted(!clickSoundsCheckBox->isChecked());
	this->app()->setAudioPlayerVolume(clickSoundsVolumeSpinBox->value());
	this->app()->setVideoSoundMuted(!videoSoundCheckBox->isChecked());
	this->app()->setVideoSoundVolume(videoSoundVolumeSpinBox->value());

	// Clips
	this->app()->setClipsDir(m_clipsDir);

	// Set all specified clip packages for the application.
	fairytale::ClipPackages clipPackages;

	foreach (ClipPackage *clipPackage, m_clipPackages.values())
	{
		clipPackages.insert(clipPackage->id(), clipPackage);
	}

	this->app()->setClipPackages(clipPackages);
}

void SettingsDialog::update()
{
	this->fullScreenCheckBox->setChecked(this->app()->isFullScreen());

	// Sound
	this->musicCheckBox->setChecked(!this->app()->isMusicMuted());
	musicVolumeSpinBox->setValue(this->app()->musicVolume());
	clickSoundsCheckBox->setChecked(!this->app()->isAudioPlayerMuted());
	clickSoundsVolumeSpinBox->setValue(this->app()->audioPlayerVolume());
	videoSoundCheckBox->setChecked(!this->app()->isVideoSoundMuted());
	videoSoundVolumeSpinBox->setValue(this->app()->videoSoundVolume());

	int i = 0;

	foreach (QAudioDeviceInfo device, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
	{
		audioOutputDeviceComboBox->addItem(device.deviceName(), device.deviceName());

		if (this->app()->audioOutputSelectorControl() != nullptr && this->app()->audioOutputSelectorControl()->activeOutput() == device.deviceName())
		{
			audioOutputDeviceComboBox->setCurrentIndex(i);
		}

		++i;
	}

	audioOutputDeviceComboBox->setEnabled(this->app()->audioOutputSelectorControl() != nullptr);

	i = 0;

	foreach (QAudioDeviceInfo device, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
	{
		audioInputDeviceComboBox->addItem(device.deviceName(), device.deviceName());

		if (this->app()->audioInputSelectorControl() != nullptr && this->app()->audioInputSelectorControl()->activeInput() == device.deviceName())
		{
			audioInputDeviceComboBox->setCurrentIndex(i);
		}

		++i;
	}

	audioInputDeviceComboBox->setEnabled(this->app()->audioInputSelectorControl() != nullptr);

	// Clips
	this->m_clipsDir = this->app()->clipsDir();
	this->clipsDirectoryLabel->setText(m_clipsDir.toString());

	this->fill(this->app()->clipPackages());
}

void SettingsDialog::addFile()
{
	const QString filePath = QFileDialog::getOpenFileName(this, tr("Add Clip"), "", tr("Clip Files (*.xml)"));

	if (!filePath.isEmpty())
	{
		ClipPackage *clipPackage = new ClipPackage(this->app());

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
	const QString dirPath = QFileDialog::getExistingDirectory(this, tr("Clips Directory"), this->app()->clipsDir().toLocalFile());

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
				ClipPackage *clipPackage = new ClipPackage(this->app());

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
            this->app()->removeClipPackage(package);

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

void SettingsDialog::itemDoubleClicked(QTreeWidgetItem *item, int /* column */)
{
	Clips::iterator iterator = m_clips.find(item);

	if (iterator != m_clips.end())
	{
		const QUrl url = this->app()->resolveClipUrl(iterator.value()->imageUrl());
		qDebug() << "Open URL: " << url;
		// TODO just popup a modal dialog with the image
		QDesktopServices::openUrl(url);
	}
	else
	{
		BonusClips::iterator bonusClipsIterator = m_bonusClips.find(item);

		if (bonusClipsIterator != m_bonusClips.end())
		{
			const QUrl url = this->app()->resolveClipUrl(bonusClipsIterator.value()->imageUrl());
			qDebug() << "Open URL: " << url;
			// TODO just popup a modal dialog with the image
			QDesktopServices::openUrl(url);
		}
	}
}

SettingsDialog::SettingsDialog(fairytale *app, QWidget *parent) : TranslatedWidget(app, parent)
{
	setupUi(this);

	connect(this->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &SettingsDialog::restoreDefaults);
	this->buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/themes/oxygen/32x32/actions/dialog-ok-apply.png"));
	this->buttonBox->button(QDialogButtonBox::Cancel)->setIconSize(QSize(32, 32));
	connect(this->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsDialog::apply);
	connect(this->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsDialog::accept);
	connect(this->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
	this->buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/themes/oxygen/32x32/actions/dialog-close.png"));
	this->buttonBox->button(QDialogButtonBox::Cancel)->setIconSize(QSize(32, 32));
	connect(this->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SettingsDialog::reject);

	connect(this->clipsDirectoryPushButton, &QPushButton::clicked, this, &SettingsDialog::changeClipsDirectory);

	connect(this->addFilePushButton, &QPushButton::clicked, this, &SettingsDialog::addFile);
	connect(this->addDirectoryPushButton, &QPushButton::clicked, this, &SettingsDialog::addDirectory);
	connect(this->removePushButton, &QPushButton::clicked, this, &SettingsDialog::removeSelected);
	connect(this->treeWidget, &QTreeWidget::itemDoubleClicked, this, &SettingsDialog::itemDoubleClicked);

	this->clipsGroupBox->setChecked(false);
	this->clipsWidget->hide();
}

void SettingsDialog::showClipsGroupBox()
{
	this->clipsGroupBox->setChecked(true);
	this->clipsWidget->show();
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

	QTreeWidgetItem *introItem = new QTreeWidgetItem(topLevelItem);
	introItem->setText(0, tr("Intro"));
	const QUrl introUrl = this->app()->resolveClipUrl(package->intro());

	if (introUrl.isLocalFile())
	{
		introItem->setIcon(0, QIcon(fairytale::filePath(introUrl)));
		introItem->setText(1, QString::number(1));
	}
	else
	{
		introItem->setIcon(0, QIcon());
		introItem->setText(1, QString::number(0));
	}

	QTreeWidgetItem *outrosItem = new QTreeWidgetItem(topLevelItem);
	outrosItem->setText(0, tr("Outros"));
	outrosItem->setText(1, QString::number(package->outros().size()));

	for (int i = 0; i < package->outros().size(); ++i)
	{
		const QUrl outroUrl = this->app()->resolveClipUrl(package->outros().at(i));
		QTreeWidgetItem *outroItem = new QTreeWidgetItem(outrosItem);

		const QString name = i < 4 ? HighScores::difficultyToString(fairytale::Difficulty(i)) : tr("Lost");
		outroItem->setText(0, name);
		outroItem->setIcon(0, QIcon(outroUrl.toLocalFile()));
	}

	QTreeWidgetItem *bonusesItem = new QTreeWidgetItem(topLevelItem);
	bonusesItem->setText(0, tr("Bonuses"));

	int persons = 0;
	int acts = 0;
	int bonuses = 0;

	for (Clip *clip : package->clips())
	{
		QTreeWidgetItem *clipItem = new QTreeWidgetItem(clip->isPerson() ? personsItem : actsItem);
		this->m_clips.insert(clipItem, clip);
		clipItem->setText(0, clip->description());
		clipItem->setIcon(0, QIcon(fairytale::filePath(this->app()->resolveClipUrl(clip->imageUrl()))));

		if (clip->isPerson())
		{
			++persons;
		}
		else
		{
			++acts;
		}
	}

	for (BonusClip *clip : package->bonusClips())
	{
		QTreeWidgetItem *clipItem = new QTreeWidgetItem(bonusesItem);
		this->m_bonusClips.insert(clipItem, clip);
		clipItem->setText(0, clip->description());
		clipItem->setIcon(0, QIcon(fairytale::filePath(this->app()->resolveClipUrl(clip->imageUrl()))));

		++bonuses;
	}

	topLevelItem->setText(1, QString::number(persons + acts));
	topLevelItem->setText(2, QString::number(package->rounds()));
	personsItem->setText(1, QString::number(persons));
	actsItem->setText(1, QString::number(acts));
	bonusesItem->setText(1, QString::number(bonuses));

	for (int i = 0; i < this->treeWidget->columnCount(); ++i)
	{
		this->treeWidget->resizeColumnToContents(i);
	}
}

void SettingsDialog::load(QSettings &settings)
{
	settings.beginGroup("settings");

	// showing maximized or fullscren leads to menu actions disappearing on a smartphone
#ifdef Q_OS_ANDROID
	const bool defaultShowFullScreen = false;
#else
	const bool defaultShowFullScreen = true;
#endif
	this->fullScreenCheckBox->setChecked(settings.value("fullscreen", defaultShowFullScreen).toBool());

	// Sound
	this->musicCheckBox->setChecked(settings.value("music", true).toBool());
	this->musicVolumeSpinBox->setValue(settings.value("musicVolume", fairytale::defaultMusicVolume).toInt());
	this->clickSoundsCheckBox->setChecked(settings.value("clickSounds", true).toBool());
	this->clickSoundsVolumeSpinBox->setValue(settings.value("clickSoundsVolume", fairytale::defaultClickSoundsVolume).toInt());
	this->videoSoundCheckBox->setChecked(settings.value("videoSound", true).toBool());
	this->videoSoundVolumeSpinBox->setValue(settings.value("videoSoundVolume", fairytale::defaultVideoSoundVolume).toInt());

	// Clips
	const QDir defaultClipsDir(this->app()->defaultClipsDirectory());
	qDebug() << "Default clips dir:" << this->app()->defaultClipsDirectory();
	// the default path is the "clips" sub directory
#ifndef Q_OS_ANDROID
	m_clipsDir = QUrl::fromLocalFile(settings.value("clipsDir", defaultClipsDir.absolutePath()).toString());
#else
	m_clipsDir = QUrl(settings.value("clipsDir", defaultClipsDir.absolutePath()).toString());
#endif
	qDebug() << "Set clips dir to" << m_clipsDir;

	const int size = settings.beginReadArray("clipPackages");

	if (size > 0)
	{
		for (int i = 0; i < size; ++i)
		{
			settings.setArrayIndex(i);
			const QString filePath = settings.value("filePath").toString();

			ClipPackage *package = new ClipPackage(this->app());

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

	// Apply before loading custom fairytales since they are not stored in the settings additionally and otherwise they would all be deleted.
	apply();

	const int customFairytalesSize = settings.beginReadArray("customfairytales");

	for (int i = 0; i < customFairytalesSize; ++i)
	{
		settings.setArrayIndex(i);
		CustomFairytale *customFairytale = new CustomFairytale(this->app());
		customFairytale->load(settings);
		this->app()->addCustomFairytale(customFairytale);
	}

	settings.endArray();

	settings.endGroup();

	// default package
	if (this->app()->clipPackages().isEmpty())
	{
		this->app()->loadDefaultClipPackage();
	}
}

void SettingsDialog::save(QSettings &settings)
{
	settings.beginGroup("settings");

	settings.setValue("fullscreen", this->fullScreenCheckBox->isChecked());

	// Sound
	settings.setValue("music", this->musicCheckBox->isChecked());
	settings.setValue("musicVolume", this->musicVolumeSpinBox->value());
	settings.setValue("clickSounds", this->clickSoundsCheckBox->isChecked());
	settings.setValue("clickSoundsVolume", this->clickSoundsVolumeSpinBox->value());
	settings.setValue("videoSound", this->videoSoundCheckBox->isChecked());
	settings.setValue("videoSoundsVolume", this->videoSoundVolumeSpinBox->value());

	// Clips
#ifndef Q_OS_ANDROID
	settings.setValue("clipsDir", m_clipsDir.toLocalFile());
#else
	settings.setValue("clipsDir", m_clipsDir.toString());
#endif

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

	for (fairytale::CustomFairytales::const_iterator iterator = this->app()->customFairytales().constBegin(); iterator != this->app()->customFairytales().constEnd(); ++iterator)
	{
		settings.setArrayIndex(i);
		iterator.value()->save(settings);

		++i;
	}

	settings.endArray();

	settings.endGroup();
}
